#include <avr/io.h>
#include <util/delay.h>

#define LED0     PB0
#define LED1     PB7
#define LED_DDR  DDRB
#define LED_PORT PORTB

#define DELAYTIME 200

#define setBit(sfr, bit)     (_SFR_BYTE(sfr) |= (1 << bit))
#define clearBit(sfr, bit)   (_SFR_BYTE(sfr) &= ~(1 << bit))
#define toggleBit(sfr, bit)  (_SFR_BYTE(sfr) ^= (1 << bit))

int main(void) {

    // Init
    LED_DDR = (1 << LED0) | (1 << LED1);  // set LED pins for output

    // Mainloop
    while (1) {
	setBit(LED_PORT, LED0);
        _delay_ms(DELAYTIME);
	clearBit(LED_PORT, LED0);
	setBit(LED_PORT, LED1);
        _delay_ms(DELAYTIME);
	clearBit(LED_PORT, LED1);
    }

    return 0;
}
