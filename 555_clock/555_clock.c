#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "USART.h"
#include "pinDefines.h"

// CLOCK DIVISION DEFINES //////////////////
#define DIV_1     (1 << CS10)
#define DIV_8     (1 << CS11)
#define DIV_64   ((1 << CS11) | (1 << CS10))
#define DIV_256   (1 << CS12)
#define DIV_1024 ((1 << CS12) | (1 << CS10))
////////////////////////////////////////////

// PIN DEFINES ///
#define CLK    PD2
#define SELECT PD3
//////////////////

// MISC DEFINES ///////////////////
#define UPDATE_INTERVAL 5000  // ms
///////////////////////////////////

volatile uint16_t aHighTicks, aLowTicks, mTicks;

uint8_t calcDutyCycle(uint16_t highTicks, uint16_t lowTicks) {
    uint8_t totalTicks = highTicks + lowTicks;

    if (!totalTicks)
        return 0;  // Avoid a division by zero

    return ((uint32_t)highTicks * 100 / totalTicks);
}

ISR(INT0_vect) {
    if (bit_is_set(PIND, SELECT)) {   // Astable operation
        if (bit_is_set(PIND, CLK)) {  // Rising edge: Get lowTicks and reset timer
            aLowTicks = TCNT1;
            TCNT1 = 0;
        } else {                      // Falling edge: Get highTicks and reset timer
            aHighTicks = TCNT1;
            TCNT1 = 0;
        }
    } else {                          // Monostable operation
        if (bit_is_set(PIND, CLK))    // Rising edge: Reset the timer
            TCNT1 = 0;
        else                          // Falling edge: Get the count
            mTicks = TCNT1;           // No need to reset the timer
    }
}

void initInterrupt0(void) {
    EIMSK |= (1 <<  INT0);  // Enable INT0 which is on PD2
    EICRA |= (1 << ISC00);  // Any change PD2 triggers the interrupt
}

static inline void initTimer1(void) {
    TCCR1B |= DIV_64;       // 8 MHz / 64 -> 125 ticks per millisecond
}

int main(void) {
    LED_DDR = 0xff;         // Let's use the LEDs as status signal

    aHighTicks = 0;
    aLowTicks = 0;
    mTicks = 0;

    initUSART();
    initTimer1();
    initInterrupt0();
    sei();                  // Enable interrupts globally

    LED_PORT = 0xff;        // All initialized and ready

    while(1) {
        _delay_ms(UPDATE_INTERVAL);      // Delay at the begining so that unit gets
                                         // enough time to calculate everything

        if (bit_is_set(PIND, SELECT)) {  // Astable Mode: Show ticks & duty cycle
            printStringLn("\nASTABLE MODE");

            printString("High Ticks: ");
            printWord(aHighTicks);

            printString("\nLow Ticks: ");
            printWord(aLowTicks);

            printByte(calcDutyCycle(aHighTicks, aLowTicks));  // TODO: Drop leading zeros
            printStringLn(" %% duty cucle recorded");

        } else {  // Monostable Mode: Show ticks and time elapsed during high time
            printString("\nMONOSTABLE MODE");

            printString("Ticks: ");
            printByte(mTicks);

            printString("\nDuration: ");
            printByte(mTicks / 125);
            printStringLn(" ms");

        }
    }

    return 0;
}
