// A demo for charliplexing LEDs

#include <avr/io.h>
#include <util/delay.h>

#define PIN_SHIFT 2

void light(uint8_t gr_idx, uint8_t led_idx) {
    // gr_idx is the position of the inactive bit
    DDRB = (~(1 << gr_idx) & 0x07) << PIN_SHIFT;
    switch (gr_idx) {
        case  0:
            PORTB = (2 << led_idx) << PIN_SHIFT;
            break;

        case 1:
            PORTB = (1 << (led_idx * 2)) << PIN_SHIFT;
            break;

        case 2:
            PORTB = (1 << led_idx) << PIN_SHIFT;
            break;

        default:
            PORTB = 0;
    }
}

int main(void) {
    while (1) {
        for (uint8_t i = 0; i < 3; i++)
            for (uint8_t j = 0; j < 2; j++) {
                light(i, j);
                _delay_ms(500);
            }
    }

    return 0;
}
