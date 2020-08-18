#include <avr/io.h>
#include <util/delay.h>

#define LED_PORT PORTB
#define LED_DDR DDRB
#define DELAY_POV 2  // ms  : Delay between two successive bytes of a pattern
#define DELAY_GAP 10  // ms : Delay between two successive instances of a pattern
#define PATTERN_LENGTH 11

uint8_t pattern1[] = {
    0b00001110,
    0b00011000,
    0b10111101,
    0b01110110,
    0b00111100,
    0b00111100,
    0b00111100,
    0b01110110,
    0b10111101,
    0b00011000,
    0b00001110,
};

uint8_t pattern2[] = {
    0b01110000,
    0b00011000,
    0b11111101,
    0b10110110,
    0b00111100,
    0b00111100,
    0b00111100,
    0b10110110,
    0b11111101,
    0b00011000,
    0b01110000,
};

void povDisplay(uint8_t oneByte) {
    LED_PORT = oneByte;
    _delay_ms(DELAY_POV);
}

void povDisplayPattern(uint8_t pattern[]) {
    for (int i = 0; i < PATTERN_LENGTH; i++)
        povDisplay(pattern[i]);

    LED_PORT = 0;
    _delay_ms(DELAY_GAP);
}

int main(void) {
    // Set all pins for output
    LED_DDR = 0xff;

    while (1) {
        // Display patterns one by one. This cascades them in a single repeating row.
        povDisplayPattern(pattern1);
        povDisplayPattern(pattern2);
    }

    return 0;
}
