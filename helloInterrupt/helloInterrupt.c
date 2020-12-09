/*
 * Flashes LED0 at a fixed rate, interrupting whenever BUTTON is pressed
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pinDefines.h"

ISR(INT0_vect) {         /* Run everytime there is a change in the button */
    if (bit_is_clear(BUTTON_PIN, BUTTON))
        LED_PORT |= (1 << LED7);
    else
        LED_PORT &= ~(1 << LED7);
}

void initInterrupt0(void) {
    EIMSK |= (1 << INT0);                                  /* enable INT0 */
    EICRA |= (1 << ISC00);                 /* trigger when button changes */
    sei();                           /* set (global) interrupt enable bit */
}

int main(void) {
    LED_DDR = 0xff;                               /* All LEDs will output */
    BUTTON_PORT |= (1 << BUTTON);       /* Enable input pullup for BUTTON */
    initInterrupt0();

    while (1) {
        _delay_ms(200);
        LED_PORT ^= (1 << LED0);
    }
    return 0;
}
