#include <avr/io.h>
#include <util/delay.h>

uint8_t get_time(uint8_t);

volatile uint8_t hours, minutes, seconds;

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

    hours   = 0x0f;
    minutes = 0x31;
    seconds = 0x00;

    DDRB = 0x3f;    // PB0-PB5 of PORTB are used as outputs
    DDRC = 0x07;    // PC0-PC2 of PORTC are used as outputs

    PORTB |= 0x3f;  // Turn PB0-PB5 on
    PORTC |= 0x01;  // Turn PC0 on;
                    // which turns on the entire hour bank

    while (1) {
        // Cycle through all three banks of LEDs
        _delay_ms(1);               // Wait a little while

        PORTB &= ~(0x3f);           // Clear 6 LSBs of PORTB

        rc = PORTC & 0x07;          // Record 3 LSBs of PORTC
        PORTC &= ~(0x07);           // Clear 3 LSBs of PORTC

        if (rc & 0x04)              // If seconds bank is on,
            rc = 0x01;              // Move back to hour bank
        else
            rc <<= 1;               // Shift to next bank

        tick();                     // The clock ticks

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
