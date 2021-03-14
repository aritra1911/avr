#include <avr/io.h>
#include <util/delay.h>
#include "USART.h"

#define MUL60(x) (((x) << 6) - ((x) << 2))

static inline void initTimer0(void) {
                                      /* Normal mode (default), just counting */
    //TCCR1B |= (1 << CS12) | (1 << CS10);
    TCCR0B |= 1 << CS00;
    TCCR0B &= ~(0x01);
    /* Clock speed: 8 MHz / 1024,
     * each tick is 128 microseconds ~= 7.8 ticks per millisecond */
                                                   /* No special output modes */
}

int main(void) {
    uint8_t ticks0, ticks1;
    uint8_t h, m, s;
    uint32_t secs;

    initUSART();
    initTimer0();

    h = 23; m = 58; s = 1;

    while (1) {
        ticks0=0;TCNT0=0;
        {
            secs = ((uint32_t)h*60 + m)*60 + s;
        }
        ticks0=TCNT0;

        printString("\nSECS : ");
        printHexByte((uint8_t)secs);
        printHexByte((uint8_t)(secs>>8 ));
        printHexByte((uint8_t)(secs>>16));
        printHexByte((uint8_t)(secs>>24));
        transmitByte('\n');

        TCNT0=0;
        {
            secs = MUL60(MUL60((uint32_t)h) + m) + s;
        }
        ticks1=TCNT0;

        printString("\nSECS : ");
        printHexByte((uint8_t)secs);
        printHexByte((uint8_t)(secs>>8));
        printHexByte((uint8_t)(secs>>16));
        printHexByte((uint8_t)(secs>>24));
        transmitByte('\n');

        printString("\nTICKS 0: ");
        printByte(ticks0);
        printString("\nTICKS 1: ");
        printByte(ticks1);
        transmitByte('\n');

        _delay_ms(1000);
    }

    return 0;
}
