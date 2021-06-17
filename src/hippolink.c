#include "hippolink.h"

#include "radio.h"
#include "tdm.h"
#include "uart.h"

#define HIPPOLINK_RADIO_REPORT_CRC_EXTRA 116
#define MSG_LEN (sizeof(struct hippolink_radio_rssi_report_s))
#define HEADER_LENGTH (sizeof(struct hippolink_header_s))
#define HIPPOLINK_MSG_ID_RADIO_RSSI_REPORT 2
#define COBS_OFFSET 1
#define COBS_OVERHEAD 2
#define CRC_LEN 2

struct hippolink_header_s {
    uint8_t msg_len;
    uint8_t node_id;
    uint8_t msg_id;
};

struct hippolink_radio_rssi_report_s {
    uint8_t remote_id;
    uint8_t rssi;
    uint8_t noise;
};

static void hippolink_fill_rssi_report();
static void hippolink_crc();
static void cobs_encode(register uint8_t hippolink_len);

extern __data uint8_t _g_node_id;
extern __data uint8_t _g_node_count;
extern __pdata uint8_t _tdm_rssi_receive_stats[TDM_MAX_RSSI_STATS];
extern __data uint8_t _tdm_rssi_noise;
__xdata uint8_t _buffer[MSG_LEN + COBS_OVERHEAD + HEADER_LENGTH];

__data uint8_t _hippolink_node_index = 0;

void hippolink_init() {
    __xdata struct hippolink_header_s *header;
    header = (struct hippolink_header_s *)&_buffer[COBS_OFFSET];
    header->msg_len = MSG_LEN;
    header->msg_id = HIPPOLINK_MSG_ID_RADIO_RSSI_REPORT;
    header->node_id = _g_node_id;
}

bool hippolink_rssi_report() {
    __data uint8_t full_packet_size;
    full_packet_size = (MSG_LEN + HEADER_LENGTH + CRC_LEN + COBS_OVERHEAD);
    if (fifo_available_mem(UART_TX_FIFO) < full_packet_size) {
        return false;
    }
    _hippolink_node_index++;
    if (_hippolink_node_index == _g_node_id) {
        _hippolink_node_index++;
    }
    if (_hippolink_node_index >= _g_node_count) {
        _hippolink_node_index = 0;
    }
    hippolink_fill_rssi_report();
    cobs_encode(full_packet_size - COBS_OVERHEAD);
    uart_write_buffered(_buffer, full_packet_size);
    _tdm_rssi_receive_stats[_hippolink_node_index] = 0;
    return true;

}

static void hippolink_fill_rssi_report() {
    struct hippolink_radio_rssi_report_s *payload;
    payload = (struct hippolink_radio_rssi_report_s
                   *)&_buffer[COBS_OFFSET + HEADER_LENGTH];
    payload->rssi = _tdm_rssi_receive_stats[_hippolink_node_index];
    payload->noise = _tdm_rssi_noise;
    payload->remote_id = _hippolink_node_index;
    hippolink_crc();
}

static void hippolink_crc() {
    __data uint16_t sum = 0xFFFF;
    __data uint8_t i, stoplen;
    stoplen = MSG_LEN + HEADER_LENGTH + COBS_OFFSET;

    _buffer[stoplen++] = HIPPOLINK_RADIO_REPORT_CRC_EXTRA;
    for (i = COBS_OFFSET; i < stoplen; i++) {
        register uint8_t tmp;
        tmp = _buffer[i] ^ (uint8_t)(sum & 0xFF);
        tmp ^= (tmp << 4);
        sum = (sum >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4);
    }
    _buffer[i - 1] = (uint8_t)(sum & 0xFF);
    _buffer[i] = (uint8_t)(sum >> 8);
}

static void cobs_encode(register uint8_t hippolink_len) {
    __data uint8_t i, offset = 0;
    _buffer[0] = 0;
    _buffer[hippolink_len + 1] = 0;
    for (i = 1; i <= hippolink_len + 1; i++) {
        offset++;
        if (!_buffer[i]) {
            _buffer[i - offset] = offset;
            offset = 0;
        }
    }
}