#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "USART.h"
#include "pinDefines.h"

// MACROS
#define TICKS_TO_MS(ticks) (ticks / 125)

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

// MISC DEFINES ///////////////////////////////
#define UPDATE_INTERVAL             5000  // ms
#define THRESHOLD_DEBOUNCE_DURATION   80  // ms
///////////////////////////////////////////////

volatile uint16_t highTicks, lowTicks;

uint8_t calcDutyCycle(uint8_t highTime, uint8_t lowTime) {
    uint8_t timePeriod = highTime + lowTime;

    if (!timePeriod)
        return 0;  // Avoid a division by zero

    return (highTime * 100) / timePeriod;
}

void monostableOut(uint16_t ticks) {
    // Show ticks, duration of pulse and check if debounced!!
    static uint8_t pulses = 0;

    printString("\nMONOSTABLE MODE (Pulse #");
    printByte(pulses++);
    printStringLn(")");

    printString("Ticks: ");
    printWord(ticks);

    uint8_t duration = TICKS_TO_MS(ticks);

    printString("\nDuration: ");
    printByte(duration);
    printString(" ms");

    if (duration < THRESHOLD_DEBOUNCE_DURATION)
        printStringLn(" (Probably bounced)");
    else
        transmitByte('\n');
}

ISR(INT0_vect) {
    if (bit_is_set(PIND, SELECT)) {   // Astable operation
        if (bit_is_set(PIND, CLK)) {  // Rising edge: Get lowTicks and reset timer
            lowTicks = TCNT1;
            TCNT1 = 0;
        } else {                      // Falling edge: Get highTicks and reset timer
            highTicks = TCNT1;
            TCNT1 = 0;
        }
    } else {                          // Monostable operation
        if (bit_is_set(PIND, CLK))    // Rising edge: Reset the timer
            TCNT1 = 0;
        else                          // Falling edge: Get the count
            monostableOut(TCNT1);     // No need to reset the timer
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

    highTicks = 0;
    lowTicks = 0;

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
            printWord(highTicks);

            printString("\nLow Ticks: ");
            printWord(lowTicks);
            transmitByte('\n');

            printByte(calcDutyCycle(TICKS_TO_MS(highTicks), TICKS_TO_MS(lowTicks)));  // TODO: Drop leading zeros
            printStringLn(" % duty cycle recorded");

        } // Monostable Mode is handled in it's own sub routine triggered by ISR()
    }

    return 0;
}
