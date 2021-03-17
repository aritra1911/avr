/*
 * Hook 32.768 kHz crystal between TOSC1 (pin 9) and TOSC2 (pin 10)
 *   NOTE: Adding 22 pF load capacitors ceases oscillation.
 * #FIXME: Crystal refuses to oscillate with the TQFP package,
 *         adding load capacitors doesn't make a difference.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "binClock.h"
#include "sync.h"

#define ADDRESS 0x54

volatile struct Time curr_time;

static inline void initTimer2(void) {
    ASSR   |= (1 << AS2);     // Enable Asynchronous clock on TOSC1
    TCCR2A |= (1 << WGM21);   // Enable Timer2 in CTC Mode
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);  // 32.768 kHz / 1024
    TIMSK2 |= (1 << OCIE2A);  // Execute interrupt if timer value matches OCR2A value
}

ISR(TIMER2_COMPA_vect) { tick(); }

void tick(void) {
    curr_time.seconds++;

    if (curr_time.seconds >= 60) {
        curr_time.minutes++;
        curr_time.seconds = 0;
    }

    if (curr_time.minutes >= 60) {
        curr_time.hours++;
        curr_time.minutes = 0;
    }

    if (curr_time.hours >= 24)
        curr_time.hours = 0;
}

int main(void) {
    uint8_t rc;

    OCR2A = 31;  // 32.768 kHz / 1024 = 32 Hz; Hence
                 // when Timer2 counts 32 ticks, that's one second
                 //    (but due to frequency inaccuracies, we intentionally tick on count 31
                 //     this will makes the clock run a bit faster but it also makes sure that
                 //     the clock doesn't lag behind severely as would happen with a count of 32)
                 // that's when the ISR() is executed and the clock ticks.


    curr_time.hours   = 0x10;
    curr_time.minutes = 0x05;
    curr_time.seconds = 0x0a;

    DDRB = 0x3f;    // PB0-PB5 of PORTB are used as outputs
    DDRC = 0x07;    // PC0-PC2 of PORTC are used as outputs

    PORTB |= 0x3f;  // Turn PB0-PB5 on
    PORTC |= 0x07;  // Turn PC0 on;
    PORTC &= ~0x01; // which turns on the entire hour bank

    initTimer2();
    initI2CSlave(ADDRESS);
    connectI2CSlave();
    sei();

    while (1) {
        // Cycle through all three banks of LEDs
        _delay_ms(5);                // Wait a little while

        PORTB &= ~(0x3f);            // Clear 6 LSBs of PORTB

        rc = (PORTC & 0x07) ^ 0x07;  // Record 3 LSBs of PORTC (inverted)
        PORTC |= (0x07);             // Clear 3 LSBs of PORTC

        if (rc & 0x04)               // If seconds bank is on,
            rc = 0x01;               // Move back to hour bank
        else
            rc <<= 1;                // Shift to next bank

        PORTC &= ~rc;                // Turn on the proper LED Bank
        PORTB |= get_time(rc);       // Display the number belonging to that bank
    }

    return 0;
}

uint8_t get_time(uint8_t pc) {
    switch (pc & 0x07) {  // Depending on 3 LSBs of PORTC, switch
        case 0x01: return curr_time.hours;    // output lines (on PORTB) to hours,
        case 0x02: return curr_time.minutes;  // minutes,
        default:   return curr_time.seconds;  // or seconds
    }
}
