/*
 * Make an external clock triggered upcounter.
 * - Triggers on rising edge
 * - Clock hooks up to PD2
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pinDefines.h"

ISR(INT0_vect) {
    if (PORTB == 0xff) {  // If we reach 255, we will start over
        // Instead play a clearing animation before starting over!
        cli();  // Make sure nothing tries to corrupt our animation
        for (uint8_t i = 0; i < 8; i++) {
            PORTB <<= 1;
            _delay_ms(100);
        }
        sei();  // Okay done
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
    initInterrupt0();

    while (1);  // Never quit

    return 0;
}
