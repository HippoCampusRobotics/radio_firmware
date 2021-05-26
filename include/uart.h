#ifndef INCLUDE_UART
#define INCLUDE_UART
#include <stdbool.h>
#include <stdint.h>

#include "fifo.h"

#define UART_BAUD_1200 1
#define UART_BAUD_2400 2
#define UART_BAUD_4800 4
#define UART_BAUD_9600 9
#define UART_BAUD_19200 19
#define UART_BAUD_38400 38
#define UART_BAUD_57600 57
#define UART_BAUD_115200 115
#define UART_BAUD_230400 230

#define UART_DEFAULT_BAUD UART_BAUD_57600
#define UART_BUFFER_SIZE 128

#define UART_TX_FIFO uart_tx_fifo
#define UART_RX_FIFO uart_rx_fifo

fifo_declare_extern(UART_TX_FIFO, UART_BUFFER_SIZE);
fifo_declare_extern(UART_RX_FIFO, UART_BUFFER_SIZE);

#define uart_interrupt_save(x) \
    {                          \
        x = ES0;               \
        ES0 = 0;               \
    }
#define uart_interrupt_restore(x) \
    { ES0 = x; }



void uart_init(register uint8_t data_rate);
bool uart_data_rate_is_valid(register uint8_t data_rate);
void uart_set_data_rate(register uint8_t data_rate);
uint8_t uart_default_baud();
int putchar(char c);
void uart_write_buffered(__xdata uint8_t* __data buffer, register uint8_t n);
uint8_t uart_read_n(register uint8_t n, __xdata uint8_t* buffer);
uint8_t uart_packets_available();
void uart_packet_counter_decrement();
void uart_packet_counter_reset();
#endif /* INCLUDE_UART */
