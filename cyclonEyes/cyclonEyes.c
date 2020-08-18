#include <avr/io.h>
#include <util/delay.h>

#define DELAY_TIME 85  // ms
#define LED_DDR DDRB
#define LED_PORT PORTB

int main(void) {
    uint8_t i = 0;

    LED_DDR = 0xff;  // Set all pins for output

    while (1) {
        // Light LEDs one by one from right to left
        while (i < 7) {
            LED_PORT = 1 << i;
            _delay_ms(DELAY_TIME);
            i++;
        }

        // Light LEDs one by one from left to right
        while (i > 0) {
            LED_PORT = 1 << i;
            _delay_ms(DELAY_TIME);
            i--;
        }
    }

    return 0;
}
