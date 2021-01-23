#include <avr/io.h>
#include <util/delay.h>

static uint8_t inline get_time(uint8_t);

volatile uint8_t hours, minutes, seconds;

static void inline tick(void) {
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

    DDRB = 0b00111111;    // PB0-PB5 of PORTB are used as outputs
    DDRC = 0b00000111;    // PC0-PC2 of PORTC are used as outputs

    PORTB |= 0b00111111;  // Turn PB0-PB5 on
    PORTC |= 0b00000001;  // Turn PC0 on;
                          // which turns on the entire hour bank

    while (1) {
        // Cycle through all three banks of LEDs
        PORTB &= ~(0b00111111);           // Clear 6 LSBs of PORTB
        PORTB |= get_time(PORTC);         // Get hours, minutes or seconds on PORTB
                                          // depending on PORTC
        _delay_ms(1);                     // Wait a little while
                                          // and then
        if (PORTC & 0b00000100) {         // If seconds bank is on,
            PORTC &= ~(0b00000111);       // Clear 3 LSBs of PORTC
            PORTC |= 0b00000001;          // Move back to hour bank
        } else {
            rc = (PORTC & 0b00000111);    // Record 3 LSBs of PORTC
            PORTC &= ~(0b00000111);       // Clear 3 LSBs of PORTC
            PORTC |= rc << 1;             // Shift 3 LSBs of PORTC
        }
        tick();
    }

    return 0;
}

static uint8_t inline get_time(uint8_t pc) {
    switch (pc & 0b00000111) {            // Depending on 3 LSBs of PORTC, switch
        case 0b00000001: return hours;    // output lines (on PORTB) to hours,
        case 0b00000010: return minutes;  // minutes,
        default: return seconds;          // or seconds
    }
}
