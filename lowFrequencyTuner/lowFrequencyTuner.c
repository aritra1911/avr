#include <avr/io.h>
#include <avr/interrupt.h>
#include "USART.h"
#include "pinDefines.h"

#define TIME_PERIOD 360  // ms

volatile uint16_t ticks;

ISR(INT0_vect) {
    // Save currrent Ticks and reset timer
    ticks = TCNT1;
    TCNT1 = 0;
    printWord(ticks);
    printString(" ticks (");
    printWord(ticks / (uint16_t)125);
    printStringLn(" ms)");
}

void initInterrupt0(void) {
    EIMSK |= (1 << INT0);  // Enable INT0
    EICRA |= (1 << ISC01);  // Trigger on falling edge
}

static inline void initTimer1(void) {
    // Start Normal Timer
    TCCR1B |= (1 << CS11) | (1 << CS10);
    // 8 MHz / 64 = 125 ticks per ms
}

int main(void) {
    initUSART();
    initInterrupt0();
    initTimer1();

    LED_DDR = 0xff;
    LED_PORT = 0xff;

    sei();

    while (1) { /* loop forever */ }

    return 0;
}
