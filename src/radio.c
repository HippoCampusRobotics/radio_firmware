#include "radio.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board_hm_trp.h"
#include "cfg.h"
#include "timer.h"

#define WAIT_BUFFER_EMPTY() while (!TXBMT1)
#define WAIT_TRANSFER() while (SPI1CFG & 0x80)
#define WAIT_SPI1_COMPLETE() while(!SPIF1)

#define NUM_DATA_RATES 13
#define NUM_RADIO_REGS 12

#define NUM_POWER_LEVELS 8

#define TX_FIFO_ALMOST_FULL_N 60
#define TX_FIFO_ALMOST_EMPTY_N 32
#define RX_FIFO_ALMOST_FULL_N 50

#define radio_interrupt_save(x) \
    {                           \
        x = EX0;                \
        EX0 = 0;                \
    }
#define radio_interrupt_restore(x) \
    { EX0 = x; }

#define RADIO_FIFO_SIZE 64

#define WAIT_FOR_TX_ACTIVE() \
    while (!(radio_register_read(EZRADIOPRO_DEVICE_STATUS) & (1 << 1)))

#define WAIT_FOR_TX_FINISHED() \
    while ((radio_register_read(EZRADIOPRO_DEVICE_STATUS) & (1 << 1)))
#define WAIT_FOR_PACKET_SENT() \
    while ((radio_register_read(EZRADIOPRO_INTERRUPT_STATUS_1) & (1 << 2)))

#define RX_INTERRUPTS \
    (EZRADIOPRO_ENRXFFAFULL | EZRADIOPRO_ENCRCERROR | EZRADIOPRO_ENPKVALID)

#define GPIO0_RX_STATE ((1 << 4) | (1 << 2) | (1 << 0))
#define GPIO1_TX_STATE ((1 << 4) | (1 << 1))
#define MODULATION_GFSK 0x03
#define FIFO_MODE 0x20

volatile __bit _g_radio_packet_received;
volatile __bit radio_preamble_detected_;
volatile uint16_t _radio_t_packet_received;
__data uint8_t radio_rx_packet_length_;
__data uint8_t radio_partial_packet_length_;
__data uint8_t last_rssi_;
// currently selected frequency hopping channel
__data uint8_t fhop_channel_;
// preamble length in nibbles
__data uint8_t _g_radio_preamble_length;
__data uint8_t radio_rx_errors;
__data uint8_t radio_tx_errors;
__xdata uint8_t radio_buffer_[RADIO_MAX_PACKET_LENGTH];
uint8_t _radio_data_rate;

static bool radio_init_phy();
static void radio_register_write(uint8_t reg, uint8_t value) __reentrant;
uint8_t radio_register_read(uint8_t reg) __reentrant;
inline static void radio_clear_interrupts();
static bool radio_soft_reset();
static void set_frequency_registers(uint32_t freq);
inline static void radio_clear_rx_fifo();
inline static void radio_clear_tx_fifo();
inline static void radio_clear_txrx_fifo();
inline static void radio_write_tx_fifo(uint8_t n, __xdata uint8_t* buf);
inline static void radio_read_rx_fifo(uint8_t n, __xdata uint8_t* buffer);

__code static const uint8_t _reg_index[NUM_RADIO_REGS] = {
    EZRADIOPRO_IF_FILTER_BANDWIDTH,
    EZRADIOPRO_CLOCK_RECOVERY_GEARSHIFT_OVERRIDE,
    EZRADIOPRO_CLOCK_RECOVERY_OVERSAMPLING_RATIO,
    EZRADIOPRO_CLOCK_RECOVERY_OFFSET_2,
    EZRADIOPRO_CLOCK_RECOVERY_OFFSET_1,
    EZRADIOPRO_CLOCK_RECOVERY_OFFSET_0,
    EZRADIOPRO_CLOCK_RECOVERY_TIMING_LOOP_GAIN_1,
    EZRADIOPRO_CLOCK_RECOVERY_TIMING_LOOP_GAIN_0,
    EZRADIOPRO_AFC_LIMITER,
    EZRADIOPRO_TX_DATA_RATE_1,
    EZRADIOPRO_TX_DATA_RATE_0,
    EZRADIOPRO_FREQUENCY_DEVIATION,
};

__code static const uint8_t power_levels[NUM_POWER_LEVELS] = {1,  2,  5,  8,
                                                              11, 14, 17, 20};
__code static const uint8_t _air_data_rates[NUM_DATA_RATES] = {
    2, 4, 8, 16, 19, 24, 32, 48, 64, 96, 128, 192, 250};

__code static const uint8_t _reg_table_433[NUM_RADIO_REGS][NUM_DATA_RATES] = {
    {0x27, 0x27, 0x27, 0x2E, 0x16, 0x01, 0x05, 0x0B, 0x9A, 0x88, 0x8A, 0x8C,
     0x8D},
    {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
     0x03},
    {0xF4, 0xFA, 0x7D, 0x3F, 0x69, 0xA7, 0x7D, 0x53, 0x5E, 0x7D, 0x5E, 0x3F,
     0x30},
    {0x20, 0x00, 0x01, 0x02, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02,
     0x02},
    {0x41, 0x83, 0x06, 0x0C, 0x37, 0xC4, 0x06, 0x89, 0x5D, 0x06, 0x5D, 0x0C,
     0xAA},
    {0x89, 0x12, 0x25, 0x4A, 0x4C, 0x9C, 0x25, 0x37, 0x86, 0x25, 0x86, 0x4A,
     0xAB},
    {0x00, 0x01, 0x02, 0x04, 0x02, 0x01, 0x02, 0x03, 0x02, 0x02, 0x02, 0x04,
     0x07},
    {0x85, 0x08, 0x0E, 0x12, 0x72, 0x8A, 0x0E, 0x18, 0xBB, 0x0E, 0xBB, 0xEA,
     0xFF},
    {0x1D, 0x1D, 0x1D, 0x1E, 0x1E, 0x1E, 0x20, 0x30, 0x41, 0x50, 0x50, 0x50,
     0x50},
    {0x10, 0x20, 0x41, 0x83, 0x9B, 0xC4, 0x08, 0x0C, 0x10, 0x18, 0x20, 0x31,
     0x40},
    {0x62, 0xC5, 0x89, 0x12, 0xA6, 0x9C, 0x31, 0x4A, 0x62, 0x93, 0xC5, 0x27,
     0x00},
    {0x03, 0x06, 0x0D, 0x1A, 0x1E, 0x26, 0x33, 0x4D, 0x66, 0x9A, 0xCD, 0xFE,
     0xFE}};

uint16_t radio_get_arrival_time() {
    register uint16_t t;
    __bit ES0_saved;
    radio_interrupt_save(ES0_saved);
    t = _radio_t_packet_received;
    radio_interrupt_restore(ES0_saved);
    return t;
}

bool radio_get_packet(uint8_t* length, __xdata uint8_t* __data buffer) {
    if (!_g_radio_packet_received) {
        return false;
    }
    *length = radio_rx_packet_length_;
    memcpy(buffer, radio_buffer_, radio_rx_packet_length_);
    radio_set_mode_receive();
    return true;
}

bool radio_preamble_detected() {
    __bit EX0_saved;
    radio_interrupt_save(EX0_saved);
    if (radio_preamble_detected_) {
        radio_interrupt_restore(EX0_saved);
        return true;
    }
    radio_interrupt_restore(EX0_saved);
    return false;
}

bool radio_packet_received() {
    __bit EX0_saved;
    radio_interrupt_save(EX0_saved);
    if (_g_radio_packet_received) {
        radio_interrupt_restore(EX0_saved);
        return true;
    }
    radio_interrupt_restore(EX0_saved);
    return false;
}

inline static void radio_clear_interrupts() {
    radio_register_read(EZRADIOPRO_INTERRUPT_STATUS_1);
    radio_register_read(EZRADIOPRO_INTERRUPT_STATUS_2);
}

inline static void radio_disable_interrupts() {
    radio_register_write(EZRADIOPRO_INTERRUPT_ENABLE_1, 0x00);
    radio_register_write(EZRADIOPRO_INTERRUPT_ENABLE_2, 0x00);
}

inline static void radio_packet_sent_interrupt_only() {
    radio_register_write(EZRADIOPRO_INTERRUPT_ENABLE_1, EZRADIOPRO_IPKSENT);
    radio_register_write(EZRADIOPRO_INTERRUPT_ENABLE_2, 0x00);
}

bool radio_init(uint8_t txpower, uint8_t num_channels, uint8_t data_rate,
                uint8_t netid) {
    bool ret;
    uint8_t channel_size;
    uint32_t freq_min = 433050000UL;
    const uint32_t freq_max = 434790000UL;

    channel_size = (freq_max / 10000 - freq_min / 10000) / (num_channels + 2);

    srand(netid);
    freq_min += ((uint32_t)(rand() * channel_size / RAND_MAX)) % channel_size;

    if (!radio_init_phy()) {
        return false;
    }
    ret = radio_configure(freq_min, data_rate, channel_size);
    if (!ret) {
        return false;
    }
    radio_set_transmit_power(txpower);
    radio_set_netid(netid);
    return true;
}

void radio_set_transmit_power(uint8_t power) {
    uint8_t i;
    for (i = 0; i < NUM_POWER_LEVELS; i++) {
        if (power <= power_levels[i]) {
            break;
        }
    }
    if (i == NUM_POWER_LEVELS) {
        i = NUM_POWER_LEVELS - 1;
    }
    radio_register_write(EZRADIOPRO_TX_POWER, i);
}

bool radio_transmit_power_is_valid(uint8_t power) {
    uint8_t i;
    for (i = 0; i < NUM_POWER_LEVELS; i++) {
        if (power == power_levels[i]) {
            return true;
        }
    }
    return false;
}

uint8_t radio_register_read(uint8_t reg) __reentrant {
    register uint8_t value;
    // save interrupt configuration and disable it
    __bit EX0_saved = EX0;
    EX0 = 0;
    SFRPAGE = LEGACY_PAGE;
    NSS1 = 0;
    SPIF1 = 0;  // clear interrupt flag
    SPI1DAT = (reg);
    WAIT_BUFFER_EMPTY();  // wait for transmit buffer to be empty
    /* value does not matter in read mode. while we are writing this, the radio
     * sends us the requested register content*/
    SPI1DAT = 0;
    WAIT_BUFFER_EMPTY();  // wait for transmit buffer to be empty
    // wait until SPIBSY is cleared (i.e until everything has been written)
    WAIT_TRANSFER();
    value = SPI1DAT;  // read the value
    SPIF1 = 0;        // clear interrupt flag
    NSS1 = 1;

    // restore interrupt state.
    EX0 = EX0_saved;

    return value;
}

static void radio_register_write(uint8_t reg, uint8_t value) __reentrant {
    __bit EX0_saved = EX0;
    EX0 = 0;

    SFRPAGE = LEGACY_PAGE;

    NSS1 = 0;
    SPIF1 = 0;
    SPI1DAT = (reg | (1 << 7));  // add the write flag to the register address
    WAIT_BUFFER_EMPTY();
    SPI1DAT = value;
    WAIT_BUFFER_EMPTY();
    WAIT_TRANSFER();
    value = SPI1DAT;
    SPIF1 = 0;
    NSS1 = 1;
}

bool radio_transmit(uint8_t len, __xdata uint8_t* buf) __reentrant {
    // store interrupt state and disbale it
    __bit EX0_saved = EX0;
    EX0 = 0;
    uint8_t n;
    if (len > sizeof(radio_buffer_)) {
        EX0 = EX0_saved;
        return false;
    }
    radio_clear_tx_fifo();
    radio_register_write(EZRADIOPRO_TRANSMIT_PACKET_LENGTH, len);

    n = len;
    if (n > RADIO_FIFO_SIZE) {
        n = RADIO_FIFO_SIZE;
    }

    radio_write_tx_fifo(n, buf);
    len -= n;
    buf += n;

    radio_packet_sent_interrupt_only();
    radio_clear_interrupts();
    radio_preamble_detected_ = 0;
    radio_register_write(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1,
                         EZRADIOPRO_TXON | EZRADIOPRO_XTON);
    WAIT_FOR_TX_ACTIVE();
    // fill the fifo chunkwise if we have a large packet to transmit
    while (len != 0) {
        static __data uint8_t status;
        status = radio_register_read(EZRADIOPRO_INTERRUPT_STATUS_1);
        if (status & EZRADIOPRO_IFFERR) {
            radio_clear_tx_fifo();
            EX0 = EX0_saved;
            return false;
        }
        if ((status & EZRADIOPRO_ITXFFAEM)) {
            // fifo is almost empty. fill it, if we have more data to send

            /* -1 is required since the almost empty interrupt is triggered as
             * soon as the remaining fifo bytes are 1 byte more than the defined
             * TX_FIFO_ALMOST_EMPTY_N threshold. */
            n = RADIO_FIFO_SIZE - 1 -
                TX_FIFO_ALMOST_EMPTY_N;  // biggest possible chunk
            // limit to how much we actually want to write.
            if (n > len) {
                n = len;
            }
            radio_write_tx_fifo(n, buf);
            len -= n;
            buf += n;
        }
    }
    WAIT_FOR_TX_FINISHED();
    radio_set_mode_receive();
    if (!(radio_register_read(EZRADIOPRO_INTERRUPT_STATUS_1) &
          EZRADIOPRO_IPKSENT)) {
        EX0 = EX0_saved;
        return false;
    }
    EX0 = EX0_saved;
    return true;
}

static bool radio_soft_reset() {
    uint8_t status;
    radio_register_write(EZRADIOPRO_INTERRUPT_ENABLE_1, 0);
    radio_register_write(EZRADIOPRO_INTERRUPT_ENABLE_2, 0);
    radio_clear_interrupts();
    radio_register_write(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1,
                         (EZRADIOPRO_SWRES | EZRADIOPRO_XTON));
    delay_set(2);
    while (IRQ) {
        if (delay_expired()) {
            return false;
        }
    }
    radio_register_write(EZRADIOPRO_INTERRUPT_ENABLE_1, 0);
    radio_register_write(EZRADIOPRO_INTERRUPT_ENABLE_2, EZRADIOPRO_ENCHIPRDY);

    delay_set(20);
    while (!delay_expired()) {
        status = radio_register_read(EZRADIOPRO_INTERRUPT_STATUS_1);
        status = radio_register_read(EZRADIOPRO_INTERRUPT_STATUS_2);
        if (status & EZRADIOPRO_ICHIPRDY) {
            return true;
        }
    }
    return false;
}

static bool radio_init_phy() {
    uint8_t status;
    delay_millis(50);
    // INT0 on P0.7 with falling edge
    IT01CF = 0x07;
    IT0 = 0;

    status = radio_register_read(EZRADIOPRO_DEVICE_VERSION);
    if (status == 0xFF || status < 5) {
        return false;
    }

    radio_soft_reset();

    status = radio_register_read(EZRADIOPRO_DEVICE_VERSION);
    if (!(status & EZRADIOPRO_IPOR)) {
        return radio_soft_reset();
    }

    if (status & EZRADIOPRO_ICHIPRDY) {
        return true;
    }
    return false;
}

bool radio_configure(uint32_t freq, uint8_t data_rate, uint8_t fhop_step_size) {
    radio_set_frequency(freq);
    radio_set_fhop_step_size(fhop_step_size);
    radio_set_fhop_channel(0);

    radio_disable_interrupts();
    radio_clear_interrupts();

    radio_register_write(EZRADIOPRO_GPIO0_CONFIGURATION, GPIO0_RX_STATE);
    radio_register_write(EZRADIOPRO_GPIO1_CONFIGURATION, GPIO1_TX_STATE);

    radio_register_write(EZRADIOPRO_CRYSTAL_OSCILLATOR_LOAD_CAPACITANCE,
                         EZRADIOPRO_OSC_CAP_VALUE);

    /* Enable RX Packet handling, Enable TX Packet handling, enable crc
     * generation, select crc-16(IBM) poly*/
    radio_register_write(EZRADIOPRO_DATA_ACCESS_CONTROL,
                         (EZRADIOPRO_ENPACTX | EZRADIOPRO_ENPACRX |
                          EZRADIOPRO_ENCRC | EZRADIOPRO_CRC_16));
    // 1 header bytes
    radio_register_write(EZRADIOPRO_HEADER_CONTROL_2,
                         EZRADIOPRO_HDLEN_1BYTE | EZRADIOPRO_SYNCLEN_2BYTE);
    // check only a single header byte
    radio_register_write(EZRADIOPRO_HEADER_CONTROL_1, (1 << 3));
    // check the whole byte
    radio_register_write(EZRADIOPRO_HEADER_ENABLE_3, 0xFF);

    // generate interrupt of more than n bytes are written to TX FIFO
    radio_register_write(EZRADIOPRO_TX_FIFO_CONTROL_1, TX_FIFO_ALMOST_FULL_N);
    radio_register_write(EZRADIOPRO_TX_FIFO_CONTROL_2, TX_FIFO_ALMOST_EMPTY_N);
    /* generate interrupt if more than the specified number of bytes in rx
     * buffer */
    radio_register_write(EZRADIOPRO_RX_FIFO_CONTROL, RX_FIFO_ALMOST_FULL_N);

    _g_radio_preamble_length = 16;
    radio_register_write(EZRADIOPRO_PREAMBLE_LENGTH, _g_radio_preamble_length);
    radio_register_write(EZRADIOPRO_PREAMBLE_DETECTION_CONTROL, 5 << 3);

    radio_set_transmit_power(0);

    // acutally this is the default setting. AFC enable and 0dB bias.
    radio_register_write(EZRADIOPRO_AFC_LOOP_GEARSHIFT_OVERRIDE, 0x44);
    radio_set_data_rate(data_rate);
    return true;
}

bool radio_data_rate_is_valid(uint8_t rate) {
    uint8_t i;
    for (i = 0; i < NUM_DATA_RATES; i++) {
        if (_air_data_rates[i] == rate) {
            return true;
        }
    }
    return false;
}

uint8_t radio_get_data_rate() { return _radio_data_rate; }

void radio_set_data_rate(uint8_t rate) {
    uint8_t i, control, rate_idx;
    // just do it. the datasheet wants us to do so. 23.3.8
    if (rate > 100) {
        radio_register_write(EZRADIOPRO_CHARGEPUMP_CURRENT_TRIMMING_OVERRIDE,
                             0xC0);
    }
    for (i = 0; i < NUM_DATA_RATES; i++) {
        if (_air_data_rates[i] >= rate) {
            break;
        }
    }
    if (i == NUM_DATA_RATES) {
        i--;
    }
    rate_idx = i;
    rate = _air_data_rates[rate_idx];
    _radio_data_rate = rate;
    // TODO: check for manchester encodinig setting and enable it if necessary

    /* enable whitening and set txtdrtscale if needed as demanded in datasheet,
     * i.e. if datarate is < 30kbps */
    if (rate >= 30) {
        control = 0x0D;
    } else {
        control = 0x2d;
    }
    radio_register_write(EZRADIOPRO_MODULATION_MODE_CONTROL_1, control);
    radio_register_write(EZRADIOPRO_MODULATION_MODE_CONTROL_2,
                         (MODULATION_GFSK | FIFO_MODE));
    if (rate < 200) {
        radio_register_write(EZRADIOPRO_AFC_TIMING_CONTROL, 0x0A);
    } else {
        radio_register_write(EZRADIOPRO_AFC_TIMING_CONTROL, 0x02);
    }

    for (i = 0; i < NUM_RADIO_REGS; i++) {
        radio_register_write(_reg_index[i], _reg_table_433[i][rate_idx]);
    }
}

bool radio_set_frequency(uint32_t freq) {
    if (freq < 240000000UL || freq > 480000000UL) {
        return false;
    }
    set_frequency_registers(freq);
    return true;
}

/**
 * @brief Sets the step size for frequency hopping als multiple of 10kHz.
 *
 * @param step_size Step size als multiple of 10kHz.
 */
void radio_set_fhop_step_size(uint8_t step_size) {
    radio_register_write(EZRADIOPRO_FREQUENCY_HOPPING_STEP_SIZE, step_size);
}

void radio_set_fhop_channel(register uint8_t channel) {
    if (channel != fhop_channel_) {
        radio_register_write(EZRADIOPRO_FREQUENCY_HOPPING_CHANNEL_SELECT,
                             channel);
        radio_preamble_detected_ = 0;
    }
}

uint8_t radio_get_fhop_channel() { return fhop_channel_; }

static void set_frequency_registers(uint32_t freq) {
    uint8_t band;
    uint16_t carrier;

    /* select the frequency band als multiple of 10MHz with an offset of 24 (see
     * data sheet 23.3.2 p.249-250)*/
    band = freq / 10000000UL - 24;
    /* carrier = (freq/10MHz - band - 24)*64000 optimized for integer division*/
    carrier = (freq / 625 - band * 16000UL - 24 * 16000UL) * 4;
    band |= EZRADIOPRO_SBSEL;
    radio_register_write(EZRADIOPRO_FREQUENCY_BAND_SELECT, band);
    radio_register_write(EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_1,
                         (uint8_t)(carrier >> 8));
    radio_register_write(EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_0,
                         (uint8_t)(carrier & 0xFF));
}

void radio_set_netid(uint8_t id) {
    radio_register_write(EZRADIOPRO_TRANSMIT_HEADER_3, id);
    radio_register_write(EZRADIOPRO_CHECK_HEADER_3, id);
}

bool radio_set_mode_receive() __reentrant {
    // disable interrupts
    EX0 = 0;
    _g_radio_packet_received = 0;
    radio_rx_packet_length_ = 0;
    radio_preamble_detected_ = 0;
    radio_partial_packet_length_ = 0;

    // enable crc, preamble, rx fifo almost full and packet received interrupts
    radio_register_write(EZRADIOPRO_INTERRUPT_ENABLE_1, RX_INTERRUPTS);
    radio_register_write(EZRADIOPRO_INTERRUPT_ENABLE_2, EZRADIOPRO_ENPREAVAL);

    radio_clear_interrupts();
    radio_clear_txrx_fifo();

    // switch on rx mode
    radio_register_write(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1,
                         EZRADIOPRO_RXON | EZRADIOPRO_XTON);
    // enable interrupts
    EX0 = 1;
    return true;
}

inline static void radio_write_tx_fifo(uint8_t n, __xdata uint8_t* buf) {
    SFRPAGE = LEGACY_PAGE;
    NSS1 = 0;
    SPIF1 = 0;
    SPI1DAT = (0x80 | EZRADIOPRO_FIFO_ACCESS);
    while (n--) {
        WAIT_BUFFER_EMPTY();
        SPI1DAT = *(buf++);
    }
    WAIT_BUFFER_EMPTY();
    WAIT_TRANSFER();
    SPIF1 = 0;
    NSS1 = 1;
}

inline static void radio_clear_rx_fifo() {
    register uint8_t control;
    control = radio_register_read(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_2);
    radio_register_write(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_2,
                         control | EZRADIOPRO_FFCLRRX);
    radio_register_write(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_2,
                         control & ~EZRADIOPRO_FFCLRRX);
}

inline static void radio_clear_tx_fifo() {
    register uint8_t control;
    control = radio_register_read(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_2);
    radio_register_write(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_2,
                         control | EZRADIOPRO_FFCLRTX);
    radio_register_write(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_2,
                         control & ~EZRADIOPRO_FFCLRTX);
}

inline static void radio_clear_txrx_fifo() {
    register uint8_t control;
    control = radio_register_read(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_2);
    radio_register_write(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_2,
                         control | (EZRADIOPRO_FFCLRRX | EZRADIOPRO_FFCLRTX));
    radio_register_write(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_2,
                         control & ~(EZRADIOPRO_FFCLRRX | EZRADIOPRO_FFCLRTX));
}

inline static void radio_read_rx_fifo(uint8_t n, __xdata uint8_t* buffer) {
    SFRPAGE = LEGACY_PAGE;
    NSS1 = 0;
    SPI1DAT = EZRADIOPRO_FIFO_ACCESS;
    WAIT_SPI1_COMPLETE();
    // see datasheet figure 22.5 SPI burst read mode why we discard the first
    // byte
    ACC = SPI1DAT;
    while (n--) {
        SPIF1 = 0;
        SPI1DAT = 0x00;
        WAIT_SPI1_COMPLETE();
        *(buffer++) = SPI1DAT;
    }
    SPIF1 = 0;
    NSS1 = 1;
}

void radio_packet_timing_test() {
    __data uint8_t rate_index, len;
    for (rate_index = 8; rate_index < NUM_DATA_RATES; rate_index++) {
        radio_set_data_rate(_air_data_rates[rate_index]);
        for (len = 0; len < 4; len++) {
            cfg_handle_cmd();
            delay_millis(50);
            PIN_CONFIG = true;
            radio_transmit(len, radio_buffer_);
            PIN_CONFIG = false;
        }
    }
}

/**
 * @brief Test packet latency for different packet lengths and data rates.
 *
 * Radio must be initialized before!
 *
 */
void radio_packet_latency_test() {
    __data uint16_t t1, t2;
    uint8_t i, rate_index;
    uint8_t buffer[RADIO_MAX_PACKET_LENGTH];
    printf(
        "######################################################################"
        "##########\nLATENCY "
        "TEST\n################################################################"
        "################\n\n");
    printf("payload_len");
    for (i = 0; i < NUM_DATA_RATES; i++) {
        printf(", %ukbs", _air_data_rates[i]);
    }
    printf("\n");
    delay_millis(500);
    for (i = 0; i < RADIO_MAX_PACKET_LENGTH; i++) {
        printf("%u", i);
        for (rate_index = 0; rate_index < NUM_DATA_RATES; rate_index++) {
            radio_set_data_rate(_air_data_rates[rate_index]);
            delay_millis(50);
            LED_RADIO = LED_ON;
            t1 = timer2_tick();
            if (!radio_transmit(i, buffer)) {
                printf("Failed to transmit\n");
            }
            t2 = timer2_tick();
            LED_RADIO = LED_OFF;
            printf(", %u", t2 - t1);
        }
        printf("\n");
    }
    printf("\n***FINISHED***\n");
}

void Receiver_ISR() __interrupt(INTERRUPT_INT0) {
    __data uint8_t status, status2, len;
    status2 = radio_register_read(EZRADIOPRO_INTERRUPT_STATUS_2);
    status = radio_register_read(EZRADIOPRO_INTERRUPT_STATUS_1);

    // RX FIFO almost full (measured according to RX_FIFO_ALMOST_FULL_N)
    if (status & EZRADIOPRO_IRXFFAFULL) {
        /* check if the message is too long to handle. should not happen in
         * normal operation */
        if ((RX_FIFO_ALMOST_FULL_N + (uint16_t)radio_partial_packet_length_) >
            RADIO_MAX_PACKET_LENGTH) {
            if (radio_rx_errors++ == 0xFF) {
                radio_rx_errors = 0xFF;
            }
            radio_set_mode_receive();
            return;
        }
        radio_read_rx_fifo(RX_FIFO_ALMOST_FULL_N,
                           &radio_buffer_[radio_partial_packet_length_]);
        radio_partial_packet_length_ += RX_FIFO_ALMOST_FULL_N;
        last_rssi_ =
            radio_register_read(EZRADIOPRO_RECEIVED_SIGNAL_STRENGTH_INDICATOR);
    }
    // preamble detected. beginning of message reception
    if (status2 & EZRADIOPRO_IPREAVAL) {
        radio_preamble_detected_ = true;
        last_rssi_ =
            radio_register_read(EZRADIOPRO_RECEIVED_SIGNAL_STRENGTH_INDICATOR);
    }
    // invalid crc -> message content is trash
    if (status & EZRADIOPRO_ICRCERROR) {
        if (radio_rx_errors++ == 0xFF) {
            radio_rx_errors = 0xFF;
        }
        radio_set_mode_receive();
        return;
    }

    // received valid packet
    if (status & EZRADIOPRO_IPKVALID) {
        _radio_t_packet_received = timer2_tick();
        PIN_CONFIG = true;
        len = radio_register_read(EZRADIOPRO_RECEIVED_PACKET_LENGTH);
        if (len > RADIO_MAX_PACKET_LENGTH ||
            radio_partial_packet_length_ > len) {
            if (radio_rx_errors++ == 0xFF) {
                radio_rx_errors = 0xFF;
            }
            radio_set_mode_receive();
            return;
        }
        if (radio_partial_packet_length_ < len) {
            radio_read_rx_fifo(len - radio_partial_packet_length_,
                               &radio_buffer_[radio_partial_packet_length_]);
        }
        radio_rx_packet_length_ = len;
        _g_radio_packet_received = true;
        // disable interrupts until the received packet has been handled
        radio_disable_interrupts();
        radio_register_write(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1,
                             EZRADIOPRO_PLLON);
    }
}