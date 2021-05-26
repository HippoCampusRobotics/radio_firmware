#ifndef TIMER
#define TIMER

#include <stdint.h>
#include <stdbool.h>

#define TIMER2_US_PER_TICK 16

void timer_init();
void delay_millis(register uint16_t millis);
void delay_set(register uint16_t msec);
bool delay_expired();
uint16_t timer2_tick();

#endif /* TIMER */
