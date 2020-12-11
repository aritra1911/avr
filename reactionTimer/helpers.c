#include "helpers.h"

void printMilliseconds(uint16_t value) {
    /* Given  a value in milliseconds, prints out how many seconds
     * you took over the serial port.  Does ascii conevrsion, prints
     * decimal point, and drops extra leading zeros.
     */
    uint8_t digit;

    printString("\nYou took ");
                                                      /* add up ten-thousands */
    digit = 0;
    while (value >= 10000) {
        digit++;
        value -= 10000;
    }
    if (digit) {
        transmitByte('0' + digit);
    }
                                                          /* add up thousands */
    digit = 0;
    while (value >= 1000) {
        digit++;
        value -= 1000;
    }
    transmitByte('0' + digit);

                                   /* decimal point here b/c dividing by 1000 */
    transmitByte('.');

                                                           /* add up hundreds */
    digit = 0;
    while (value >= 100) {
        digit++;
        value -= 100;
    }
    transmitByte('0' + digit);

                                                               /* add up tens */
    digit = 0;
    while (value >= 10) {
        digit++;
        value -= 10;
    }
    transmitByte('0' + digit);

    // One digit is easy
    transmitByte('0' + value);

    printString(" seconds.\n");
}

void printComments(uint16_t value) {
                   /* Given a value in milliseconds, rates your reaction time */
    if (value > 2000)
        printString("----> Umm...this is a reaction timer...\n");
    else if (value > 1000)
        printString("----> Hello?\n");
    else if (value > 500)
        printString("----> Slow.\n");
    else if (value > 250)
        printString("----> Have another cup of coffee.\n");
    else if (value > 200)
        printString("----> Respectable.\n");
    else if (value >= 150)
        printString("----> Fast\n");
    else if (value < 150)
        printString("----> Amazing!\n");
}

void randomDelay(void) {
                              /* waits for a "random" delay from 1 to 3.5 sec */
                                  /* Requires timer 1 initialized and running */
    /* It's not really random, but very hard to control --
     * like coin-flipping. */
    uint8_t randomTime;

    _delay_ms(1000);                                   /* wait at least 1 sec */
    randomTime = (uint8_t) TCNT1;
    /* type-casting the 16-bit TCNT1 as an 8-bit number keeps
     * only the 8 least-significant (fastest-changing) bits */
    while (--randomTime)
        _delay_ms(10);                         /* max value is 255 ~= 2.5 sec */
}
