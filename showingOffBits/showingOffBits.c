#include <avr/io.h>
#include <util/delay.h>

#define DELAY_TIME 85  // ms
#define RANDOM_FLIPS 75
#define RANDOM_SEED 0x1234
#define LED_DDR DDRB
#define LED_PORT PORTB

uint8_t getRandomLED(uint16_t* randomNumber) {
    // A quick and dirty random number generator.
    *randomNumber = 2053 * *randomNumber + 13849;
    return (*randomNumber >> 8) & 0b00000111;  // Get the most significant 8 bits and
                                               // return the least significant three bits of it
                                               // which is guaranteed to be between 0 and 7
}

int main(void) {
    uint8_t i;
    uint16_t randomNumber = RANDOM_SEED;

    LED_DDR = 0xff;  // Set all pins for output
    LED_PORT = 0;    // Start out with all LEDs off

    while (1) {
        // Move block of 8 LEDs from right to left
        for (i = 0; i < 8; i++) {
            LED_PORT |= (1 << i);
            _delay_ms(DELAY_TIME);
        }
        for (i = 0; i < 8; i++) {
            LED_PORT &= ~(1 << i);
            _delay_ms(DELAY_TIME);
        }

        _delay_ms(5 * DELAY_TIME);  // pause

        // Move block of 8 LEDs from left to right
        // In 8-bit integer arithmetic,
        //     255 = 0 - 1
        for (i = 7; i < 255; i--) {
            LED_PORT |= (1 << i);
            _delay_ms(DELAY_TIME);
        }
        for (i = 7; i < 255; i--) {
            LED_PORT &= ~(1 << i);
            _delay_ms(DELAY_TIME);
        }

        _delay_ms(5 * DELAY_TIME);  // pause

        // Flip random LEDs
        for (i = 0; i < RANDOM_FLIPS; i++) {
            LED_PORT ^= 1 << getRandomLED(&randomNumber);
            _delay_ms(DELAY_TIME);
        }

        LED_PORT = 0;               // Turn off all LEDs
        _delay_ms(5 * DELAY_TIME);  // pause
    }
    return 0;
}
