/*
 * Make an external clock triggered upcounter.
 * - Triggers on rising edge
 * - Clock hooks up to PD2
 * - Wastes exactly 7 clock cycles in clearing
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pinDefines.h"

volatile uint8_t clearing;

ISR(INT0_vect) {
    // Keep incrementing PORTB, but
    // when it reaches 255,
    // play a clearing animation (right to left)
    // which also resets PORTB to 0.
    if (clearing) {
        PORTB <<= 1;
        if (!PORTB) clearing = 0;
    } else if (PORTB == 0xff) {
        clearing = 1;
        PORTB <<= 1;
    } else
        PORTB++;
}

void initInterrupt0(void) {
    EIMSK |= (1 << INT0);
    EICRA |= (1 << ISC01) | (1 << ISC00);  // Interrupt on rising edge
    sei();
}

int main(void) {
    LED_DDR = 0xff;  // Everyone must output
    LED_PORT = 0x00;  // Everyone must be silent, initially
    clearing = 0;
    initInterrupt0();

    while (1);  // Never quit! Let interrupts handle everything!

    return 0;
}
