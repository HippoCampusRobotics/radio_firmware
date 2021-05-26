#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "board_hm_trp.h"
#include "cfg.h"
#include "flash.h"
#include "radio.h"
#include "tdm.h"
#include "timer.h"
#include "uart.h"
#include "version.h"

#define __STRING(x) #x
#define STRING(x) __STRING(x)

__at(FLASH_SIGNATURE_BYTES) uint8_t __code app_signature[2] = {FLASH_SIG0,
                                                               FLASH_SIG1};

extern void Serial_ISR() __interrupt(INTERRUPT_UART0);
extern void Receiver_ISR() __interrupt(INTERRUPT_INT0);
extern void T2_ISR() __interrupt(INTERRUPT_TIMER2);
extern void T3_ISR() __interrupt(INTERRUPT_TIMER3);

static void hardware_init();
static void spi_init();
static void adc_init();
inline static void led_init();

__code const char banner_string_[] =
    "HippoLink MultiPoint " STRING(VERSION_MAJOR) "." STRING(VERSION_MINOR);
__code const char version_string_[] =
    STRING(VERSION_MAJOR) "." STRING(VERSION_MINOR);

enum BoardFrequency board_frequency_;
uint8_t board_bl_version_;

__data uint8_t _g_node_id;
__data uint8_t _g_node_count;

void main() {
    uint8_t i;
    bool state = true;
    board_frequency_ = BOARD_FREQUENCY_REG;
    board_bl_version_ = BOARD_BL_VERSION_REG;
    hardware_init();
    LED_RADIO = LED_ON;
    LED_BOOTLOADER = LED_ON;
    delay_millis(2000);
    LED_BOOTLOADER = LED_OFF;
    _g_node_count = cfg_param_get(CFG_PARAM_NODECOUNT);
    _g_node_id = cfg_param_get(CFG_PARAM_NODEID);
    tdm_init();
    tdm_run();
    // while (1) {
    //     radio_packet_timing_test();
    // }
}

static void hardware_init() {
    uint16_t i;
    SFRPAGE = LEGACY_PAGE;

    // disble watchdog
    PCA0MD &= ~0x40;

    // select internal oscillator without prescaling.
    OSCICN = 0x8F;
    CLKSEL = 0x00;
    FLSCL = 0x40;

    // brown out
    VDM0CN = 0x80;
    for (i = 0; i < 350; i++)
        ;
    RSTSRC = 0x06;

    led_init();
    LED_RADIO = LED_OFF;
    LED_BOOTLOADER = LED_OFF;
    SFRPAGE = LEGACY_PAGE;

    // uart
    cfg_param_load();
    uart_init(cfg_param_get(CFG_PARAM_UART_BAUD));

    spi_init();

    SFRPAGE = LEGACY_PAGE;
    IE0 = 0;  // clear radio interrupt state

    timer_init();

    // set all interrupts to the same priority -> no nested interrupts possible
    IP = 0;
    EA = 1;  // enable interrupts globally.

    adc_init();

    SFRPAGE = LEGACY_PAGE;

    // enable crossbar 2, i.e. pins on P2
    XBR2 = (1 << 6);

    radio_init(cfg_param_get(CFG_PARAM_TXPOWER),
               cfg_param_get(CFG_PARAM_NUM_CHANNELS),
               cfg_param_get(CFG_PARAM_RADIO_DATA_RATE),
               cfg_param_get(CFG_PARAM_NETID));
}

inline static void led_init() {
    SFRPAGE = LEGACY_PAGE;
    P1MDOUT |= 0x60;
    SFRPAGE = CONFIG_PAGE;
    P1DRV |= 0x60;
}

inline static void spi_init() {
    SFRPAGE = LEGACY_PAGE;
    XBR1 |= (1 << 6);  // enable SPI
    // TODO: Verify this is correct. Original firmware sets another value.
    P1MDOUT |= (0xF0 | (1 << 0) | (1 << 2));  // SCK1 and MOSI1 set to push-pull
    SFRPAGE = CONFIG_PAGE;
    P0DRV |= 0x04;                          // CONFIG PIN P0.2 in push-pull mode
    P1DRV |= (0xF0 | (1 << 0) | (1 << 2));  // SCK1 and MOSI1 high current mode
    // TODO: why are we doing this?
    P2DRV = 0xFF;
    SFRPAGE = LEGACY_PAGE;
    SPI1CFG = (1 << 6);                // master mode
    SPI1CN &= ~((1 << 3) | (1 << 2));  // 3 wire mode
    SPI1CKR = 0;                       // set prescaler to minimum of 2
    SPI1CN |= 0x01;                    // enable SPI
    NSS1 = 1;
}

inline static void adc_init() {
    SFRPAGE = LEGACY_PAGE;
    AD0EN = 1;                      // Enable adc0
    ADC0CF |= (1 << 2) | (1 << 0);  // 8bit mode and gain=1
    ADC0MX = 0x1B;  // connect adc0 with the interal temperature sensor via the
                    // multiplexer
    /**
     * 0=1: enable internal voltage reference output 1.68V at P0.0
     * 1: unused
     * 2=1: enable temperature sensor
     * 3-4=00: select P0.0 as reference */
    REF0CN = (1 << 0) | (1 << 1) | (1 << 2);
}
