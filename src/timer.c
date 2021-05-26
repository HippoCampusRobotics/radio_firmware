#include "timer.h"

#include <stdbool.h>
#include <stdint.h>

#include "board_hm_trp.h"
#include "cfg.h"

#define TIMER3_PRESCALER 12  // DO NOT MODIFY
#define TIMER3_HZ 100        // DO NOT MODIFY
#define TIMER3_RELOAD \
    UINT16_C(UINT16_MAX - SYSCLK / TIMER3_PRESCALER / TIMER3_HZ + 1)

static __data volatile uint8_t _timer2_overflows;

static __data volatile uint8_t _delay_counter;

void T3_ISR() __interrupt(INTERRUPT_TIMER3) {
    // clear interrupt flag
    TMR3CN &= ~(1 << 7);
    cfg_mode_fsm_update_time();

    if (_delay_counter) {
        _delay_counter--;
    }
}

void T2_ISR() __interrupt(INTERRUPT_TIMER2) {
    // clear interrupt flag
    TF2H = 0;
    _timer2_overflows++;
}

void delay_set(register uint16_t msec) {
    if (msec >= 2550) {
        _delay_counter = 255;
    } else {
        // convert msec in 1/100 seconds.
        _delay_counter = (msec + 9) / 10;
    }
}

bool delay_expired() { return _delay_counter == 0; }

/**
 * @brief Delay for a minimum of the specified milli seconds.
 *
 * Actual resolution depends on timer3 interrupt period and is 10ms; Delay
 * period is rounded up.
 *
 * @param millis Minimum delay time. Can not be greater than 2.55 seconds.
 */
void delay_millis(register uint16_t millis) {
    delay_set(millis);
    while (!delay_expired())
        ;
}

static inline uint16_t timer2_16() {
    register uint8_t low, high;
    do {
        high = TMR2H;
        low = TMR2L;
    } while (high != TMR2H);
    return low | (((uint16_t)high) << 8);
}

#pragma save
#pragma nooverlay
uint16_t timer2_tick() {
    register uint8_t high;
    register uint16_t low;
    do {
        high = _timer2_overflows;
        low = timer2_16();
    } while (high != _timer2_overflows);
    return (uint16_t)(high << 11) | (low >> 5);
}
#pragma restore

void timer_init() {
    SFRPAGE = LEGACY_PAGE;
    // set reload registers to reload with frequency f=TIMER3_HZ
    TMR3RLL = (TIMER3_RELOAD)&0xFF;
    TMR3RLH = ((TIMER3_RELOAD) >> 8) & 0xFF;
    // enable timer
    TMR3CN = (1 << 2);
    // enable timer3 interrupt
    EIE1 |= (1 << 7);

    // run timer2 at SYSCLK / 12
    TMR2RLL = 0;
    TMR2RLH = 0;
    // enable timer2
    TMR2CN = (1 << 2);
    // enable timer 2 interrupt
    ET2 = 1;
}