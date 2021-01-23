#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

uint8_t get_time(uint8_t);
void tick(void);

volatile uint8_t hours, minutes, seconds;
volatile uint16_t millis;

static inline void initTimer0(void) {
    TCCR0A |= (1 << WGM01);               // Enable Timer0 in CTC Mode
    TCCR0B |= (1 << CS01) | (1 << CS00);  // clk/64 (From prescaler)
    TIMSK0 |= (1 << OCIE0A);              // Execute interrupt if timer value matches OCR0A value
}

ISR(TIMER0_COMPA_vect) {
    millis++;

    if (millis >= 1000) {
        tick();
        millis = 0;
    }
}

void tick(void) {
    seconds++;

    if (seconds >= 60) {
        minutes++;
        seconds = 0;
    }

    if (minutes >= 60) {
        hours++;
        minutes = 0;
    }

    if (hours >= 24)
        hours = 0;
}

int main(void) {
    uint8_t rc;

    OCR0A = 125;  // ISR() called at an interval of 1 millisecond since
                  // 8 MHz / 64 = 125 ticks per millisecond
    millis = 0;   // TODO: Can this be static inside the ISR()?

    initTimer0();
    sei();

    hours   = 0x12;
    minutes = 0x06;
    seconds = 0x00;

    DDRB = 0x3f;    // PB0-PB5 of PORTB are used as outputs
    DDRC = 0x07;    // PC0-PC2 of PORTC are used as outputs

    PORTB |= 0x3f;  // Turn PB0-PB5 on
    PORTC |= 0x01;  // Turn PC0 on;
                    // which turns on the entire hour bank

    while (1) {
        // Cycle through all three banks of LEDs
        _delay_ms(5);               // Wait a little while

        PORTB &= ~(0x3f);           // Clear 6 LSBs of PORTB

        rc = PORTC & 0x07;          // Record 3 LSBs of PORTC
        PORTC &= ~(0x07);           // Clear 3 LSBs of PORTC

        if (rc & 0x04)              // If seconds bank is on,
            rc = 0x01;              // Move back to hour bank
        else
            rc <<= 1;               // Shift to next bank

        PORTC |= rc;                // Turn on the proper LED Bank
        PORTB |= get_time(rc);      // Display the number belonging to that bank
    }

    return 0;
}

uint8_t get_time(uint8_t pc) {
    switch (pc & 0x07) {            // Depending on 3 LSBs of PORTC, switch
        case 0x01: return hours;    // output lines (on PORTB) to hours,
        case 0x02: return minutes;  // minutes,
        default: return seconds;    // or seconds
    }
}
