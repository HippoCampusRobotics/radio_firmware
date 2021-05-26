#ifndef INCLUDE_CFG
#define INCLUDE_CFG

#include <stdbool.h>
#include <stdint.h>

#include "radio.h"
#include "uart.h"

#define CFG_CMD_MAX_LEN 16

typedef uint8_t cfg_param_t;

typedef enum {
    CFG_PARAM_UART_BAUD = 0,
    CFG_PARAM_RADIO_DATA_RATE,
    CFG_PARAM_NUM_CHANNELS,
    CFG_PARAM_TXPOWER,
    CFG_PARAM_NETID,
    CFG_PARAM_NODEID,
    CFG_PARAM_NODECOUNT,
    CFG_PARAM_MANCHESTER,
    CFG_PARAM_LBT_RSSI,
    CFG_PARAM_MAX
} cfg_param_id_t;

typedef enum {
    CFG_MODE_STATE_SILENCE_BEFORE,
    CFG_MODE_STATE_SILENCE_AFTER,
    CFG_MODE_STATE_RECEIVE_SYMBOL,
    CFG_MODE_STATE_ACTIVE
}cfg_mode_state_t;

extern __code const struct cfg_param_info_s cfg_param_info_[];
extern __bit _cfg_mode_active;
extern __data uint8_t cfg_mode_symbol_cntr_;

cfg_param_t cfg_param_get(cfg_param_id_t id);
bool cfg_param_set(cfg_param_id_t id, cfg_param_t value);
void cfg_param_reset_to_default();
void cfg_param_write(cfg_param_id_t id, uint16_t address_offset);
void cfg_param_save();
bool cfg_param_load();
bool cfg_param_is_valid(cfg_param_id_t id, cfg_param_t param);

void cfg_mode_fsm_update_char(register uint8_t c);
void cfg_mode_fsm_update_time();
void cfg_handle_cmd();

#endif /* INCLUDE_CFG */
