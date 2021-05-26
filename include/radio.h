#ifndef INCLUDE_RADIO
#define INCLUDE_RADIO

#include <stdbool.h>
#include <stdint.h>

#include "board_hm_trp.h"

#define RADIO_NUM_CHANNELS_MAX 50
#define RADIO_NUM_CHANNELS_MIN 10
#define RADIO_NUM_CHANNELS_DEFAULT RADIO_NUM_CHANNELS_MIN
#define RADIO_MAX_PACKET_LENGTH 64
#define RADIO_DEFAULT_DATA_RATE 64
#define RADIO_DEFAULT_TXPOWER 11

extern __data uint8_t _g_radio_preamble_length;

bool radio_init(uint8_t txpower, uint8_t num_channels, uint8_t data_rate,
                uint8_t netid);
bool radio_configure(uint32_t freq, uint8_t data_rate, uint8_t fhop_step_size);
bool radio_set_frequency(uint32_t freq);
void radio_set_fhop_step_size(uint8_t step_size);
void radio_set_fhop_channel(register uint8_t channel);
uint8_t radio_get_fhop_channel();
bool radio_transmit(uint8_t len, __xdata uint8_t* buf) __reentrant;
void radio_set_netid(uint8_t id);
uint8_t radio_get_data_rate();
void radio_set_data_rate(uint8_t rate);
bool radio_data_rate_is_valid(uint8_t rate);
bool radio_transmit_power_is_valid(uint8_t power);
void radio_set_transmit_power(uint8_t power);
bool radio_packet_received();
bool radio_preamble_detected();
bool radio_get_packet(uint8_t *length, __xdata uint8_t * __data buffer);
uint16_t radio_get_arrival_time();
bool radio_set_mode_receive();
void radio_packet_timing_test();

#endif /* INCLUDE_RADIO */
