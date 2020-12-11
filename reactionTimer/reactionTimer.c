/*
 * Press the button as quickly as you can after the LEDs light up.
 * Your time is printed out over the serial port.
 */
#include <avr/io.h>
#include "USART.h"
#include "pinDefines.h"
#include "helpers.h"

static inline void initTimer1(void) {
                                      /* Normal mode (default), just counting */
    TCCR1B |= (1 << CS12) | (1 << CS10);
    /* Clock speed: 8 MHz / 1024,
     * each tick is 128 microseconds ~= 7.8 ticks per millisecond */
                                                   /* No special output modes */
}

int main(void) {
    volatile char byte;
    uint16_t timerValue;

    initUSART();
    initTimer1();
    LED_DDR = 0xff;                                    /* all LEDs for output */
    BUTTON_PORT |= (1 << BUTTON);                  /* enable internal pull-up */

    printStringLn("\nReactionTimer:");
    printStringLn("---------------");
    printStringLn("Press any key to start.");

    while(1) {
        byte = receiveByte();                                /* press any key */
        transmitByte(byte);

        printStringLn("\nGet ready...");
        randomDelay();

        if (bit_is_clear(BUTTON_PIN, BUTTON)) {
                                               /* BUTTON is already held down */
            printStringLn("I won't proceed until you let go of that button.");
            loop_until_bit_is_set(BUTTON_PIN, BUTTON);
            randomDelay();
        }


        printStringLn("\nGo!");
        LED_PORT = 0xff;                                        /* light LEDs */
        TCNT1 = 0;                                           /* reset counter */

        if (bit_is_clear(BUTTON_PIN, BUTTON)) {
                   /* BUTTON pressed _exactly_ as LEDs light up.  Suspicious. */
            printStringLn("You're only chaeting yourself.\n");
        } else {
            // Wait until button pressed, save time value.
            loop_until_bit_is_clear(BUTTON_PIN, BUTTON);
            timerValue = TCNT1 >> 3;
              /* each tick is approx 1/8 milliseconds, so we bit-shift divide */
            printMilliseconds(timerValue);
            printComments(timerValue);
        }

        // Clear LEDs and start again.
        LED_PORT = 0x00;
        printStringLn("Press any key to try again.");
    }

    return 0;
}
