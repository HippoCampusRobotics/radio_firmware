#include "tdm.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "board_hm_trp.h"
#include "cfg.h"
#include "hippolink.h"
#include "pkt.h"
#include "radio.h"
#include "timer.h"
#include "uart.h"

#define TDM_SYNC_SLOT(slot_count) (slot_count - 1)
#define TDM_SLOT_COUNT(node_count) (node_count + 1)
#define TDM_NODE_COUNT(slot_count) (slot_count - 1)
#define TDM_BASE_NODE 0

#define TDM_LINK_UPDATE_PERIOD 31250

#define TDM_SWITCH_NEXT_SLOT(slot, slot_count) \
    do {                                       \
        if (slot + 1 == slot_count) {          \
            slot = 0;                          \
        } else {                               \
            slot++;                            \
        }                                      \
    } while (0)
// 2 crc + 1 len + 2 sync + 1 header = 6 bytes
#define TDM_OVERHEAD_BYTES 6

#define TDM_SYNC_FLAG 0x80
#define TDM_IS_SYNC(id) (id & 0x80)
#define TDM_REMOVE_SYNC_FLAG(id) (id & ~TDM_SYNC_FLAG))

#define TDM_TICKS_PER_BYTE(data_rate)                                  \
    ((8 * 1000000UL / (data_rate * 1000UL) + TIMER2_US_PER_TICK - 1) / \
     TIMER2_US_PER_TICK)
#define TDM_US_PER_BYTE(data_rate) (8 * 1000000UL / (data_rate * 1000UL) + 1)

#define TDM_TX_DELAY_TICKS 13

inline static void tdm_extract_trailer(register uint8_t packet_len);
inline void tdm_populate_trailer();

extern __data uint8_t _radio_last_rssi;
__pdata uint8_t _tdm_rssi_receive_stats[TDM_MAX_RSSI_STATS];
__data uint8_t _tdm_rssi_noise;

__bit _tdm_in_sync = false;
__bit _tdm_config_pin_state = false;
__bit _tdm_is_base_node;
__bit _tdm_is_in_silence_period = true;
__bit _tdm_led_radio_state = LED_OFF;
__bit _tdm_led_bootloader_state = LED_OFF;
__bit _tdm_packet_sent = false;
static __data uint8_t _tdm_sync_hops;
static __bit _tdm_sync_received = false;
static __data uint8_t _tdm_sync_count = 0;

__data struct tdm_trailer_s _tdm_trailer;

__data uint8_t test_counter = 0;
__data uint8_t _tdm_slot_count;
__data uint8_t _tdm_current_slot;
__data uint16_t _tdm_t_remaining;
__data uint16_t _tdm_t_next_slot;
__data uint16_t _tdm_t_last, _tdm_t_now;
extern __data uint8_t _g_node_id;
extern __data uint8_t _g_node_count;
extern uint8_t radio_register_read(uint8_t reg);
__xdata uint8_t _tdm_buffer[RADIO_MAX_PACKET_LENGTH];

__data struct tdm_pkt_timing_s {
    uint8_t ticks_per_byte;
    uint16_t min_latency;
    uint16_t max_latency;
    uint16_t silence_ticks;
    uint16_t slot_ticks;
} _tdm_pkt_timing;

void tdm_init() {
    hippolink_init();
    _tdm_slot_count = TDM_SLOT_COUNT(_g_node_count);
    _tdm_current_slot = TDM_SYNC_SLOT(_tdm_slot_count);
    uint8_t data_rate = radio_get_data_rate();
    _tdm_pkt_timing.ticks_per_byte = TDM_TICKS_PER_BYTE(data_rate);
    _tdm_pkt_timing.min_latency =
        ((_g_radio_preamble_length + 1) / 2 + TDM_OVERHEAD_BYTES) *
            _tdm_pkt_timing.ticks_per_byte +
        TDM_TX_DELAY_TICKS;
    _tdm_pkt_timing.max_latency =
        _tdm_pkt_timing.min_latency +
        _tdm_pkt_timing.ticks_per_byte * RADIO_MAX_PACKET_LENGTH;
    _tdm_pkt_timing.silence_ticks = _tdm_pkt_timing.min_latency;
    _tdm_pkt_timing.slot_ticks =
        _tdm_pkt_timing.silence_ticks + _tdm_pkt_timing.max_latency;
    _tdm_is_base_node = (_g_node_id == TDM_BASE_NODE);
}

inline static void tdm_extract_trailer(register uint8_t packet_len) {
    memcpy(&_tdm_trailer, _tdm_buffer + packet_len, sizeof(_tdm_trailer));
}

void handle_received_packet(register uint8_t packet_len) {
    __data uint16_t dt;
    packet_len -= sizeof(_tdm_trailer);
    tdm_extract_trailer(packet_len);
    if (TDM_IS_SYNC(_tdm_trailer.node_id)) {
        /* handle base node's sync package in the dedicated sync slot -> best
         * sync we can get */
        if (!(++_tdm_sync_count)) {
            _tdm_sync_count = 0xFF;
        }
        _tdm_t_now = timer2_tick();
        _tdm_t_last = _tdm_t_now;
        PIN_CONFIG = false;
        // dt = _tdm_t_now - radio_get_arrival_time();
        _tdm_t_remaining =
            _tdm_pkt_timing.slot_ticks - 158 - _tdm_pkt_timing.silence_ticks;
        _tdm_sync_received = true;
        _tdm_sync_hops = 1;
        _tdm_current_slot = TDM_SYNC_SLOT(_tdm_slot_count);
        // TODO: uncomment for normal function
        _tdm_in_sync = true;
        radio_set_mode_receive();
    } else if (packet_len && !_cfg_mode_active) {
        // HANDLE USER DATA
        uart_write_buffered(_tdm_buffer, packet_len);
    }
    if (_tdm_trailer.node_id < TDM_MAX_RSSI_STATS) {
        _tdm_rssi_receive_stats[_tdm_trailer.node_id] = _radio_last_rssi;
    }
}

inline void tdm_indicate_slot_change() {
    LED_BOOTLOADER = _tdm_led_bootloader_state;
    _tdm_led_bootloader_state = !_tdm_led_bootloader_state;
}

void tdm_update_slot() {
    register __data uint16_t dt;
    dt = _tdm_t_now - _tdm_t_last;
    _tdm_t_last = _tdm_t_now;
    if (dt >= _tdm_t_remaining) {
        dt -= _tdm_t_remaining;
        _tdm_t_remaining = _tdm_pkt_timing.slot_ticks - dt;
        if (_tdm_in_sync) {
            _tdm_packet_sent = false;
            TDM_SWITCH_NEXT_SLOT(_tdm_current_slot, _tdm_slot_count);
            tdm_indicate_slot_change();
        } else {
            _tdm_current_slot = TDM_SYNC_SLOT(_tdm_slot_count);
        }
    } else {
        _tdm_t_remaining -= dt;
    }
    if (_tdm_t_remaining + _tdm_pkt_timing.silence_ticks <
        _tdm_pkt_timing.slot_ticks) {
        _tdm_is_in_silence_period = false;
    } else {
        _tdm_is_in_silence_period = true;
    }
}

inline void tdm_update_link() {
    if (_tdm_sync_received || _tdm_is_base_node) {
        _tdm_sync_received = false;
        LED_RADIO = LED_ON;
    } else {
        _tdm_sync_count = 0;
        LED_RADIO = _tdm_led_radio_state;
        _tdm_led_radio_state = !_tdm_led_radio_state;
        _tdm_in_sync = false;
    }
}

void tdm_transmit_sync() {
    _tdm_trailer.node_id = TDM_SYNC_FLAG;
    _tdm_trailer.sync_hops = 0;
    memcpy(_tdm_buffer, &_tdm_trailer, sizeof(_tdm_trailer));
    radio_transmit(sizeof(_tdm_trailer), _tdm_buffer);
}

inline void tdm_populate_trailer() {
    if (_g_node_id == TDM_BASE_NODE) {
        _tdm_trailer.sync_hops = 0;
    } else {
        _tdm_trailer.sync_hops = _tdm_sync_hops;
    }
    _tdm_trailer.node_id = _g_node_id;
}

void tdm_run() {
    uint8_t i;
    __bit send_rssi = true;
    __data uint8_t tdm_counter = 0;
    __data uint8_t packet_len;
    __data uint16_t t_link_update;
    radio_set_mode_receive();
    _tdm_t_now = timer2_tick();
    _tdm_t_last = t_link_update = _tdm_t_now;
    _tdm_t_remaining = _tdm_pkt_timing.slot_ticks;
    _tdm_t_next_slot = _tdm_t_now + _tdm_t_remaining;
    _tdm_in_sync = _tdm_is_base_node;
    while (true) {
        cfg_handle_cmd();
        if (_tdm_t_now - t_link_update > TDM_LINK_UPDATE_PERIOD) {
            t_link_update = _tdm_t_now;
            tdm_update_link();
        }
        pkt_update_buffer();
        _tdm_t_now = timer2_tick();
        tdm_update_slot();
        if (radio_get_packet(&packet_len, _tdm_buffer)) {
            // it is not our turn to receive, so listen for incoming messages
            handle_received_packet(packet_len);
        } else if (_tdm_current_slot == _g_node_id && _tdm_in_sync &&
                   !_tdm_packet_sent && !_tdm_is_in_silence_period) {
            tdm_populate_trailer();
            _tdm_packet_sent = true;
            pkt_send_packets(&_tdm_trailer);
            _tdm_rssi_noise = radio_current_rssi();
            tdm_counter = 0;
            send_rssi = true;
        } else if (_tdm_current_slot == TDM_SYNC_SLOT(_tdm_slot_count)) {
            if (_tdm_is_base_node) {
                if (!_tdm_is_in_silence_period && !_tdm_packet_sent) {
                    _tdm_packet_sent = true;
                    PIN_CONFIG = true;
                    tdm_transmit_sync();
                    PIN_CONFIG = false;
                }
            }
            if (!_cfg_mode_active && send_rssi) {
                send_rssi = hippolink_rssi_report();
                tdm_counter++;
                if (tdm_counter >= TDM_MAX_RSSI_STATS ||
                    tdm_counter >= _g_node_count) {
                    send_rssi = false;
                }
            }
        }
    }
}
