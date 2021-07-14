#include "cfg.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "flash.h"
#include "radio.h"
#include "timer.h"
#include "uart.h"

#define CFG_PARSER_SYMBOL '+'
#define CFG_PARSER_SYMBOL_COUNT 5
// 10ms ticks from 100Hz timer3
#define CFG_MODE_SILENCE_TICKS 20

#define PRINT_PARAM_ARGS(id, name, value) "P%hd(%s)=%hd\n", id, name, value

__data uint8_t cfg_mode_silence_cntr_ = CFG_MODE_SILENCE_TICKS;
__bit _cfg_mode_active = false;
__bit _cfg_cmd_completed = false;

static __data uint8_t _parser_idx;
static __data char _cfg_cmd[CFG_CMD_MAX_LEN + 1];
static __data uint8_t _cfg_cmd_len = 0;
__data uint8_t cfg_mode_symbol_cntr_ = 0;
static __data cfg_mode_state_t _cfg_mode_state = CFG_MODE_STATE_SILENCE_BEFORE;

__code const struct cfg_param_info_s {
    const char *name;
    cfg_param_t default_value;
} cfg_param_info_[CFG_PARAM_MAX] = {
    {"UART_BAUD", UART_DEFAULT_BAUD},
    {"RADIO_DATA_RATE", RADIO_DEFAULT_DATA_RATE},
    {"NUM_CHANNELS", RADIO_NUM_CHANNELS_DEFAULT},
    {"TXPOWER", RADIO_DEFAULT_TXPOWER},
    {"NETID", 25},
    {"NODEID", 1},
    {"NODECOUNT", 10},
    {"MANCHESTER", 0},
    {"LBT_RSSI", 0},
};

inline static void cfg_mode_transition_silence_before();
inline static void cfg_mode_transition_silence_after();
inline static void cfg_mode_transition_receive_symbol();
inline static void cfg_mode_transition_active();
inline static void cfg_handle_char_enter();
inline static void cfg_handle_char_delete();
inline static void cfg_cmd_ok();
inline static void cfg_cmd_err();
static void cfg_print_all_params();
static uint8_t cfg_str_to_int();
static void cfg_cmd_handle_param();
inline static void cfg_cmd_handle_reboot();
inline static void cfg_cmd_handle_exit();
inline static void cfg_cmd_handle_save();
inline static void cfg_cmd_handle_reset();
inline static void cfg_cmd_handle_version();
static void cfg_cmd_handle_load();

cfg_param_t _cfg_params[CFG_PARAM_MAX];

cfg_param_t cfg_param_get(cfg_param_id_t id) { return _cfg_params[id]; }

inline static void cfg_cmd_ok() { printf("OK\n"); }

inline static void cfg_cmd_err() { printf("ERR\n"); }

static uint8_t cfg_str_to_int() __reentrant {
    uint8_t val = 0;
    while (_cfg_cmd[_parser_idx] >= '0' && _cfg_cmd[_parser_idx] <= '9') {
        val = val * 10 + (_cfg_cmd[_parser_idx] - '0');
        _parser_idx++;
    }
    return val;
}

static void cfg_print_all_params() {
    uint8_t i;
    for (i = 0; i < CFG_PARAM_MAX; i++) {
        printf(PRINT_PARAM_ARGS(i, cfg_param_info_[i].name, _cfg_params[i]));
    }
}

bool cfg_param_set(cfg_param_id_t id, cfg_param_t value) {
    if (!cfg_param_is_valid(id, value)) {
        return false;
    }
    _cfg_params[id] = value;
    return true;
}

void cfg_param_write(cfg_param_id_t id, uint16_t address_offset) {
    uint8_t i;
    __xdata uint8_t *ptr = (__xdata uint8_t *)&(_cfg_params[id]);
    uint16_t address = address_offset + id * sizeof(cfg_param_t);
    for (i = 0; i < sizeof(cfg_param_t); i++) {
        flash_scratchpad_write(address, *(ptr + i));
    }
}

void cfg_param_read(cfg_param_id_t id, uint16_t address_offset) {
    uint8_t i;
    __xdata uint8_t *ptr = (__xdata uint8_t *)(&_cfg_params[id]);
    uint16_t address = address_offset + id * sizeof(cfg_param_t);
    for (i = 0; i < sizeof(cfg_param_t); i++) {
        *(ptr + i) = flash_scratchpad_read(address + i);
    }
}

void cfg_param_reset_to_default() {
    cfg_param_id_t id;
    for (id = 0; id < CFG_PARAM_MAX; id++) {
        _cfg_params[id] = cfg_param_info_[id].default_value;
    }
}

void cfg_param_save() __critical {
    cfg_param_id_t id;
    flash_scratchpad_erase();
    flash_scratchpad_write(FLASH_SCRATCHPAD_ADDRESS, sizeof(_cfg_params));
    for (id = 0; id < CFG_PARAM_MAX; id++) {
        cfg_param_write(id, FLASH_SCRATCHPAD_ADDRESS + 1);
    }
}

bool cfg_param_is_valid(cfg_param_id_t id, cfg_param_t param) {
    switch (id) {
        case CFG_PARAM_UART_BAUD:
            return uart_data_rate_is_valid(param);
        case CFG_PARAM_RADIO_DATA_RATE:
            return radio_data_rate_is_valid(param);
        case CFG_PARAM_NUM_CHANNELS:
            return ((param >= RADIO_NUM_CHANNELS_MIN) &&
                    (param <= RADIO_NUM_CHANNELS_MAX));
        case CFG_PARAM_TXPOWER:
            return radio_transmit_power_is_valid(param);
        case CFG_PARAM_NETID:
            // there is no wrong netid
            return true;
        case CFG_PARAM_NODEID:
            return (param < _cfg_params[CFG_PARAM_NODECOUNT] &&
                    param < INT8_MAX);
        case CFG_PARAM_NODECOUNT:
            return (param > _cfg_params[CFG_PARAM_NODEID] && param >= 2 &&
                    param < INT8_MAX);
        case CFG_PARAM_MANCHESTER:
            return true;
        case CFG_PARAM_LBT_RSSI:
            return true;
        default:
            return false;
    }
}

bool cfg_param_load() __critical {
    uint8_t i, len;
    cfg_param_reset_to_default();
    len = flash_scratchpad_read(FLASH_SCRATCHPAD_ADDRESS);
    if (len != sizeof(_cfg_params)) {
        return false;
    }
    for (i = 0; i < CFG_PARAM_MAX; i++) {
        cfg_param_read(i, FLASH_SCRATCHPAD_ADDRESS + 1);
        if (!cfg_param_is_valid(i, _cfg_params[i])) {
            _cfg_params[i] = cfg_param_info_[i].default_value;
        }
    }
    return true;
}

#pragma save
#pragma nooverlay
/**
 * @brief Update the cfg cli parser state machine. ONLY call from an
 * interrupt routine.
 *
 * @param c Character from the serial interface.
 */
void cfg_mode_fsm_update_char(register uint8_t c) {
    switch (_cfg_mode_state) {
        case CFG_MODE_STATE_SILENCE_AFTER:
            _cfg_mode_state = CFG_MODE_STATE_SILENCE_BEFORE;
        case CFG_MODE_STATE_SILENCE_BEFORE:
            cfg_mode_silence_cntr_ = CFG_MODE_SILENCE_TICKS;
            break;

        case CFG_MODE_STATE_RECEIVE_SYMBOL:
            if (c == (uint8_t)CFG_PARSER_SYMBOL) {
                if (++cfg_mode_symbol_cntr_ >= CFG_PARSER_SYMBOL_COUNT) {
                    cfg_mode_transition_silence_after();
                }
            } else {
                // received wrong symbol. wait a certain time until we try
                // again.
                cfg_mode_transition_silence_before();
            }
            break;

        case CFG_MODE_STATE_ACTIVE:
            // we are in cfg mode. handle the input accordingly.
            if (_cfg_cmd_completed) {
                return;
            }
            switch (c) {
                case '\r':
                    putchar('\n');
                    _cfg_cmd[_cfg_cmd_len] = '\0';
                    _cfg_cmd_completed = true;
                    break;
                case '\b':
                case '\x7f':
                    if (_cfg_cmd_len) {
                        putchar('\b');
                        putchar(' ');
                        putchar('\b');
                        _cfg_cmd_len--;
                    }
                    break;
                default:
                    if (_cfg_cmd_len < CFG_CMD_MAX_LEN) {
                        if (isprint(c)) {
                            c = toupper(c);
                            _cfg_cmd[_cfg_cmd_len++] = c;
                            putchar(c);
                        }
                    } else {
                        cfg_mode_transition_silence_before();
                    }
                    break;
            }
            break;
    }
}
#pragma restore

#pragma save
#pragma nooverlay
void cfg_mode_fsm_update_time() {
    // check if timer is active and has just run out.
    if (cfg_mode_silence_cntr_ > 0 && !(--cfg_mode_silence_cntr_)) {
        switch (_cfg_mode_state) {
            case CFG_MODE_STATE_SILENCE_BEFORE:
                cfg_mode_transition_receive_symbol();
                break;
            case CFG_MODE_STATE_SILENCE_AFTER:
                cfg_mode_transition_active();
                break;
            default:
                break;
        }
    }
}
#pragma restore

inline static void cfg_mode_transition_silence_before() {
    _cfg_mode_active = false;
    _cfg_mode_state = CFG_MODE_STATE_SILENCE_BEFORE;
    cfg_mode_silence_cntr_ = CFG_MODE_SILENCE_TICKS;
}

inline static void cfg_mode_transition_silence_after() {
    _cfg_mode_state = CFG_MODE_STATE_SILENCE_AFTER;
    cfg_mode_silence_cntr_ = CFG_MODE_SILENCE_TICKS;
}

inline static void cfg_mode_transition_receive_symbol() {
    _cfg_mode_state = CFG_MODE_STATE_RECEIVE_SYMBOL;
    cfg_mode_symbol_cntr_ = 0;
}

inline static void cfg_mode_transition_active() {
    _cfg_mode_active = true;
    _cfg_mode_state = CFG_MODE_STATE_ACTIVE;
    _cfg_cmd[0] = '\0';
    _cfg_cmd_len = 0;
    _cfg_cmd_completed = true;
}

inline static void cfg_handle_char_enter() {
    putchar('\n');
    _cfg_cmd[_cfg_cmd_len] = '\0';
    _cfg_cmd_completed = true;
}

inline static void cfg_handle_char_delete() {
    if (_cfg_cmd_len) {
        putchar('\b');
        putchar(' ');
        putchar('\b');
        _cfg_cmd_len--;
    }
}

void cfg_handle_cmd() {
    if (!_cfg_cmd_completed) {
        return;
    }
    _cfg_cmd_completed = false;
    if (_cfg_cmd[0] == '\0') {
        cfg_cmd_ok();
    } else if (_cfg_cmd[0] == 'P') {
        if (_cfg_cmd_len >= 2) {
            cfg_cmd_handle_param();
        } else {
            cfg_cmd_err();
        }
    } else if (!strcmp(_cfg_cmd, "FLASH") || !strcmp(_cfg_cmd, "UPDATE") ||
               !strcmp(_cfg_cmd, "BOOTLOADER")) {
        flash_error_reset();
    } else if (!strcmp(_cfg_cmd, "LOAD")) {
        cfg_cmd_handle_load();
    } else if (!strcmp(_cfg_cmd, "SAVE") || !strcmp(_cfg_cmd, "STORE")) {
        cfg_cmd_handle_save();
    } else if (!strcmp(_cfg_cmd, "RESET")) {
        cfg_cmd_handle_reset();
    } else if (!strcmp(_cfg_cmd, "REBOOT")) {
        cfg_cmd_handle_reboot();
    } else if (!strcmp(_cfg_cmd, "EXIT")) {
        cfg_cmd_handle_exit();
    } else if (!strcmp(_cfg_cmd, "VERSION")) {
        cfg_cmd_handle_version();
    } else {
        cfg_cmd_err();
    }
    _cfg_cmd_len = 0;
}

static void cfg_cmd_handle_param() {
    cfg_param_id_t id;
    cfg_param_t val;
    _parser_idx = 1;
    if (_cfg_cmd[_parser_idx] == '?') {
        cfg_print_all_params();
        cfg_cmd_ok();
    } else {
        id = cfg_str_to_int();
        if (_cfg_cmd[_parser_idx] == '?') {
            printf(PRINT_PARAM_ARGS(id, cfg_param_info_[id].name,
                                    _cfg_params[id]));
            cfg_cmd_ok();
        } else if (_cfg_cmd[_parser_idx] == '=') {
            _parser_idx++;
            val = cfg_str_to_int();
            if (!cfg_param_set(id, val)) {
                cfg_cmd_err();
            } else {
                cfg_cmd_ok();
            }
        }
    }
}

inline static void cfg_cmd_handle_reboot() {
    cfg_cmd_ok();
    delay_millis(100);
    RSTSRC |= 0x10;
    for (;;)
        ;
}

inline static void cfg_cmd_handle_exit() {
    cfg_cmd_ok();
    cfg_mode_transition_silence_before();
}

inline static void cfg_cmd_handle_save() {
    cfg_param_save();
    printf("Reboot is required for changes to take effect.\n");
    cfg_cmd_ok();
}

inline static void cfg_cmd_handle_reset() {
    cfg_param_reset_to_default();
    cfg_cmd_ok();
}

static void cfg_cmd_handle_load() {
    uint8_t i, len;
    len = flash_scratchpad_read(FLASH_SCRATCHPAD_ADDRESS);
    printf(
        "Expected params to take %hd bytes of memory. Actually stored %hd "
        "bytes.\n",
        sizeof(_cfg_params), len);
    for (i = 0; i < CFG_PARAM_MAX; i++) {
        cfg_param_read(i, FLASH_SCRATCHPAD_ADDRESS + 1);
        printf("Loaded: ");
        printf(PRINT_PARAM_ARGS(i, cfg_param_info_[i].name, _cfg_params[i]));
        if (!cfg_param_is_valid(i, _cfg_params[i])) {
            _cfg_params[i] = cfg_param_info_[i].default_value;
            printf("Value is invalid. Set to default.\n");
        }
    }
}

inline static void cfg_cmd_handle_version() {
    printf("Date: %s\n", BUILD_DATE);
    delay_millis(500);
    printf("Commit: %s\n", BUILD_COMMIT);
}
