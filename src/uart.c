#include "uart.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "board_hm_trp.h"
#include "cfg.h"
#include "fifo.h"

#define PACKET_DELIMITER 0

static volatile __bit tx_idle;

volatile __data uint8_t _n_packets_received = 0;
__data uint8_t _uart_unhandled_packets = 0;

fifo_create(UART_TX_FIFO, UART_BUFFER_SIZE);
fifo_create(UART_RX_FIFO, UART_BUFFER_SIZE);

static const __code struct {
    uint8_t rate;
    uint8_t th1;
    uint8_t ckcon;
} _uart_rates[] = {
    {UART_BAUD_1200, 0x2C, 0x02},    // 1200
    {UART_BAUD_2400, 0x96, 0x02},    // 2400
    {UART_BAUD_4800, 0x2C, 0x00},    // 4800
    {UART_BAUD_9600, 0x96, 0x00},    // 9600
    {UART_BAUD_19200, 0x60, 0x01},   // 19200
    {UART_BAUD_38400, 0xb0, 0x01},   // 38400
    {UART_BAUD_57600, 0x2b, 0x08},   // 57600 - default
    {UART_BAUD_115200, 0x96, 0x08},  // 115200
    {UART_BAUD_230400, 0xcb, 0x08},  // 230400
};

inline static void _uart_write(register uint8_t c);

uint8_t uart_packets_available() {
    __bit ES0_saved;
    register uint8_t n;
    uart_interrupt_save(ES0_saved);
    n = _n_packets_received;
    uart_interrupt_restore(ES0_saved);
    return n;
}

void uart_packet_counter_decrement() {
    __bit ES0_saved;
    uart_interrupt_save(ES0_saved);
    _n_packets_received--;
    uart_interrupt_restore(ES0_saved);
}

void uart_packet_counter_reset() {
    __bit ES0_saved;
    uart_interrupt_save(ES0_saved);
    _n_packets_received = 0;
    uart_interrupt_restore(ES0_saved);
}

void Serial_ISR() __interrupt(INTERRUPT_UART0) {
    register uint8_t c;
    // receive interrupt
    if (RI0) {
        RI0 = 0;
        c = SBUF0;
        cfg_mode_fsm_update_char(c);
        if (!_cfg_mode_active && !fifo_is_full(UART_RX_FIFO)) {
            fifo_write_unsave(UART_RX_FIFO, c);
            if (c == PACKET_DELIMITER) {
                _n_packets_received++;
            }
        }
    }

    if (TI0) {
        TI0 = 0;
        if (fifo_not_empty(UART_TX_FIFO)) {
            /* no need to read save. we've already checked that the fifo is not
             * empty. */
            fifo_read_unsave(UART_TX_FIFO, SBUF0);
        } else {
            tx_idle = true;
        }
    }
}

void uart_init(register uint8_t data_rate) {
    SFRPAGE = LEGACY_PAGE;
    P0MDOUT = (1 << 4);  // tx output mode is push-pull
    SFRPAGE = CONFIG_PAGE;
    P0DRV = (1 << 4);  // tx drive strength set to high
    SFRPAGE = LEGACY_PAGE;
    XBR0 = 0x01;  // enable UART
    ES0 = 0;
    fifo_init(UART_TX_FIFO);
    fifo_init(UART_RX_FIFO);
    tx_idle = true;

    TR1 = 0;
    TMOD = (TMOD & ~0xf0) | 0x20;
    uart_set_data_rate(data_rate);
    TR1 = 1;
    // enable rx
    SCON0 = 0x10;
    ES0 = 1;
}

uint8_t uart_default_baud() { return UART_DEFAULT_BAUD; }

void uart_write_buffered(__xdata uint8_t* __data buffer, register uint8_t n) {
    __data uint8_t n_available;
    __data uint8_t n_chunk;
    __bit ES0_saved;

    if (n == 0) {
        return;
    }

    /* available mem depends on rx index, that gets modfied in UART interrupts,
     * so disable them */
    uart_interrupt_save(ES0_saved);
    n_available = fifo_available_mem(UART_TX_FIFO);
    uart_interrupt_restore(ES0_saved);
    n = (n > n_available) ? n_available : n;

    /* bytes until buffer wraps depends only on tx index and we are the only one
     * modifying it -> no interrupt disables needed for reading its value*/
    n_chunk = fifo_n_until_writewrap(UART_TX_FIFO);
    if (n_chunk <= n) {
        /* we have to write over the ring buffer wrap around, so memcpy has to
         * be split up in two segments */

        // write until the end of the ring buffer
        memcpy(&FIFO_AT_WRITE(UART_TX_FIFO), buffer, n_chunk);
        buffer += n_chunk;
        n -= n_chunk;
        // write remaining bytes at the start of the buffer
        memcpy(FIFO_BUFFER(UART_TX_FIFO), buffer, n);
        uart_interrupt_save(ES0_saved);
        // modify write index and level manually
        FIFO_WRITE_IDX(UART_TX_FIFO) = n;
        FIFO_LEVEL(UART_TX_FIFO) += (n + n_chunk);
    } else {
        /* No ring buffer wrap around will occur, so we only have to execute
         * memcpy once. */
        memcpy(&FIFO_AT_WRITE(UART_TX_FIFO), buffer, n);
        uart_interrupt_save(ES0_saved);
        FIFO_WRITE_IDX(UART_TX_FIFO) += n;
        FIFO_LEVEL(UART_TX_FIFO) += n;
    }
    if (tx_idle) {
        tx_idle = false;
        TI0 = 1;
    }
    uart_interrupt_restore(ES0_saved);
}

uint8_t uart_read_n(register uint8_t n, __xdata uint8_t* buffer) {
    __bit ES0_saved;
    __data uint8_t n_available, n_chunk, bytes_to_write;
    uart_interrupt_save(ES0_saved);
    n_available = fifo_available_data(UART_RX_FIFO);
    uart_interrupt_restore(ES0_saved);
    if (n_available == 0) {
        return 0;
    }
    bytes_to_write = (n > n_available) ? n_available : n;
    n_chunk = fifo_n_until_readwrap(UART_RX_FIFO);
    if (n_chunk <= n) {
        /* we want to read bytes over the ring buffer wrap around, so split
         * memcpy into two parts */
        memcpy(buffer, &FIFO_AT_READ(UART_RX_FIFO), n_chunk);
        buffer += n_chunk;
        bytes_to_write -= n_chunk;
        // read the remaining bytes from the beginning of the buffer array
        memcpy(buffer, FIFO_BUFFER(UART_RX_FIFO), bytes_to_write);
        uart_interrupt_save(ES0_saved);
        FIFO_READ_IDX(UART_RX_FIFO) = bytes_to_write;
        FIFO_LEVEL(UART_RX_FIFO) -= n;
        uart_interrupt_restore(ES0_saved);
    } else {
        memcpy(buffer, &FIFO_AT_READ(UART_RX_FIFO), n);
        FIFO_READ_IDX(UART_RX_FIFO) += n;
        uart_interrupt_save(ES0_saved);
        FIFO_LEVEL(UART_RX_FIFO) -= n;
        uart_interrupt_restore(ES0_saved);
    }
    return n;
}

uint8_t uart_tx_available_mem() {
    register uint8_t ret;
    __bit ES0_saved = 0;
    uart_interrupt_save(ES0_saved);
    ret = fifo_available_mem(UART_TX_FIFO);
    uart_interrupt_save(ES0_saved);
    return ret;
}



bool uart_data_rate_is_valid(register uint8_t data_rate) {
    uint8_t i;
    for (i = 0; i < LEN(_uart_rates); i++) {
        if (data_rate == _uart_rates[i].rate) {
            return true;
        }
    }
    return false;
}

void uart_set_data_rate(register uint8_t data_rate) {
    uint8_t i;
    if (!uart_data_rate_is_valid(data_rate)) {
        data_rate = uart_default_baud();
    }
    for (i = 0; i < LEN(_uart_rates); i++) {
        if (data_rate == _uart_rates[i].rate) {
            break;
        }
    }
    TH1 = _uart_rates[i].th1;
    CKCON = (CKCON & ~0x0B) | _uart_rates[i].ckcon;
}

int putchar(char c) __reentrant {
    __bit ES0_saved = 0;
    uart_interrupt_save(ES0_saved);
    if (c == '\n') {
        _uart_write('\r');
    }
    _uart_write(c);
    uart_interrupt_restore(ES0_saved);
    return 0;
}

inline static void _uart_write(register uint8_t c) {
    if (fifo_not_full(UART_TX_FIFO)) {
        fifo_write_unsave(UART_TX_FIFO, c);
        if (tx_idle) {
            tx_idle = false;
            // set interrupt flag, so the interrupt handler starts sending.
            TI0 = 1;
        }
    }
}
