#include "pkt.h"

#include <stdio.h>
#include <string.h>

#include "board_hm_trp.h"
#include "fifo.h"
#include "radio.h"
#include "tdm.h"
#include "timer.h"
#include "uart.h"

__xdata uint8_t _pkt_buffer[PKT_BUFFER_SIZE];
__pdata pkt_manager_t _pkt_manager;

void pkt_init() { memset(&_pkt_manager, 0, sizeof(_pkt_manager)); }

void pkt_send_packets(struct tdm_trailer_s *trailer) {
    __data uint8_t i, size;
    if (_pkt_manager.n_packets > 0) {
        size = _pkt_manager.sizes[0] + sizeof(struct tdm_trailer_s);
        for (i = 1; i < _pkt_manager.n_packets; i++) {
            if (size + _pkt_manager.sizes[i] > RADIO_MAX_PACKET_LENGTH) {
                break;
            }
            size += _pkt_manager.sizes[i];
        }
        memcpy(_pkt_buffer + size - sizeof(struct tdm_trailer_s), trailer,
               sizeof(struct tdm_trailer_s));
    } else {
        // transmit an empty packet to signal we are alive
        size = sizeof(struct tdm_trailer_s);
        memcpy(_pkt_buffer, trailer, size);
    }

    radio_transmit(size, _pkt_buffer);
    _pkt_manager.n_packets = 0;
}

void pkt_handle_packet(register uint8_t len) {
    register uint8_t id;
    __data uint8_t i;
    id = fifo_peek(UART_RX_FIFO, PKT_MSG_ID_IDX);
    for (i = 0; i < _pkt_manager.n_packets; i++) {
        if (_pkt_manager.ids[i] == id) {
            // we already have a version of this packet in the buffer. replace
            // it
            if (_pkt_manager.sizes[i] == len) {
                uart_read_n(len, _pkt_buffer + _pkt_manager.offsets[i]);
                return;
            } else {
                // TODO: this should never happen. warn the user if it does
                return;
            }
        }
    }
    if (_pkt_manager.n_packets < PKT_MAX_NO) {
        _pkt_manager.ids[_pkt_manager.n_packets] = id;
        _pkt_manager.sizes[_pkt_manager.n_packets] = len;
        if (_pkt_manager.n_packets == 0) {
            _pkt_manager.offsets[_pkt_manager.n_packets] = 0;
        } else {
            _pkt_manager.offsets[_pkt_manager.n_packets] =
                _pkt_manager.offsets[_pkt_manager.n_packets - 1] +
                _pkt_manager.sizes[_pkt_manager.n_packets - 1];
        }
        uart_read_n(len,
                    _pkt_buffer + _pkt_manager.offsets[_pkt_manager.n_packets]);
        _pkt_manager.n_packets++;
    }
}

void pkt_update_buffer() {
    __data uint8_t len;
    __data uint8_t id;
    __bit success;
    __bit ES0_saved;
    if (uart_packets_available()) {
        uart_packet_counter_decrement();
        len = fifo_peek(UART_RX_FIFO, PKT_LEN_IDX);
        len += PKT_HEADER_LENGTH + 1 + PKT_CRC_LENGTH;
        if (fifo_peek(UART_RX_FIFO, len) == 0) {
            pkt_handle_packet(len + 1);
            return;

        } else {
            len = fifo_available_mem(UART_RX_FIFO);
            success = false;
            for (id = 0; id < len; id++) {
                if (fifo_peek(UART_RX_FIFO, id) == 0) {
                    success = true;
                    id++;
                    uart_interrupt_save(ES0_saved);
                    fifo_remove(UART_RX_FIFO, id);
                    uart_interrupt_restore(ES0_saved);
                    break;
                }
            }
            if (!success) {
                // TODO: this should never happen. warn the user
                uart_interrupt_save(ES0_saved);
                fifo_reset(UART_RX_FIFO);
                uart_packet_counter_reset();
                uart_interrupt_restore(ES0_saved);
            }
        }
    } else {
        uart_interrupt_save(ES0_saved);
        if (fifo_is_full(UART_RX_FIFO)) {
            fifo_reset(UART_RX_FIFO);
        }
        uart_interrupt_restore(ES0_saved);
    }
}