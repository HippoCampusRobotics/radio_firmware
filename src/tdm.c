#include "tdm.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "board_hm_trp.h"
#include "cfg.h"
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
    {                                          \
        if (slot + 1 == slot_count) {          \
            slot = 0;                          \
        } else {                               \
            slot++;                            \
        }                                      \
    }
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

__bit _tdm_connected = false;
__bit _tdm_config_pin_state = false;
__bit _tdm_is_base_node;
__bit _tdm_is_in_silence_period = true;
__bit _tdm_led_radio_state = LED_OFF;
__bit _tdm_led_bootloader_state = LED_OFF;
__bit _tdm_packet_sent = false;
static __bit _sync_received = false;
static __data uint8_t _sync_count = 0;

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
    memcpy(&_tdm_trailer, _tdm_buffer + packet_len,
           sizeof(_tdm_trailer));
}

void handle_received_packet(register uint8_t packet_len) {
    __data uint16_t dt;
    packet_len -= sizeof(_tdm_trailer);
    tdm_extract_trailer(packet_len);
    if (TDM_IS_SYNC(_tdm_trailer.node_id)) {
        // HANDLE SYNC PACKAGE
        if (!(++_sync_count)) {
            _sync_count = 0xFF;
        }
        _tdm_t_now = timer2_tick();
        _tdm_t_last = _tdm_t_now;
        dt = _tdm_t_now - radio_get_arrival_time();
        _tdm_t_remaining = _tdm_pkt_timing.slot_ticks - 152 - dt -
                           _tdm_pkt_timing.silence_ticks;
        _sync_received = true;
        _tdm_current_slot = TDM_SYNC_SLOT(_tdm_slot_count);
        _tdm_connected = true;
        radio_set_mode_receive();
    } else if (packet_len && !_cfg_mode_active) {
        // HANDLE USER DATA
        uart_write_buffered(_tdm_buffer, packet_len);
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
        if (_tdm_connected) {
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
    if (_sync_received || _tdm_is_base_node) {
        _sync_received = false;
        LED_RADIO = LED_ON;
    } else {
        _sync_count = 0;
        LED_RADIO = _tdm_led_radio_state;
        _tdm_led_radio_state = !_tdm_led_radio_state;
        _tdm_connected = false;
    }
}

void tdm_transmit_sync() {
    // if (test_counter++ < 10) {
    //     return;
    // }
    // test_counter = 0;
    _tdm_trailer.node_id = TDM_SYNC_FLAG;
    memcpy(_tdm_buffer, &_tdm_trailer, sizeof(_tdm_trailer));
    radio_transmit(sizeof(_tdm_trailer), _tdm_buffer);
}

void tdm_run() {
    uint8_t i;
    __data uint8_t packet_len;
    __data uint16_t t_link_update;
    radio_set_mode_receive();
    _tdm_t_now = timer2_tick();
    _tdm_t_last = t_link_update = _tdm_t_now;
    _tdm_t_remaining = _tdm_pkt_timing.slot_ticks;
    _tdm_t_next_slot = _tdm_t_now + _tdm_t_remaining;
    _tdm_connected = _tdm_is_base_node;
    while (true) {
        cfg_handle_cmd();
        if (_tdm_t_now - t_link_update > TDM_LINK_UPDATE_PERIOD) {
            t_link_update = _tdm_t_now;
            tdm_update_link();
        }
        pkt_update_buffer();
        _tdm_t_now = timer2_tick();
        tdm_update_slot();
        if (_tdm_current_slot == _g_node_id && !_tdm_packet_sent && !_tdm_is_in_silence_period) {
            _tdm_packet_sent = true;
            _tdm_trailer.node_id = _g_node_id;
            pkt_send_packets(&_tdm_trailer);
        } else if (radio_get_packet(&packet_len, _tdm_buffer)) {
            // it is not our turn to receive, so listen for incoming messages
            handle_received_packet(packet_len);
        } else if (_tdm_current_slot == TDM_SYNC_SLOT(_tdm_slot_count) &&
                   _tdm_is_base_node) {
            if (!_tdm_is_in_silence_period && !_tdm_packet_sent) {
                _tdm_packet_sent = true;
                tdm_transmit_sync();
            }
        }
    }
}
