#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"

#define CLOCK_ADDRESS   0x54
#define CLOCK_ADDRESS_W (CLOCK_ADDRESS << 1)
#define CLOCK_ADDRESS_R (CLOCK_ADDRESS_W | 0x01)

struct Time {
    uint8_t hours
          , minutes
          , seconds;
};

void tick(void);

volatile struct Time curr_time;

void tick(void) {
    curr_time.seconds++;

    if (curr_time.seconds >= 60) {
        curr_time.minutes++;
        curr_time.seconds = 0;
    }

    if (curr_time.minutes >= 60) {
        curr_time.hours++;
        curr_time.minutes = 0;
    }

    if (curr_time.hours >= 24)
        curr_time.hours = 0;
}

int main(void) {
    initI2C();

    curr_time.hours   = 16;
    curr_time.minutes = 36;
    curr_time.seconds =  6;

    while (1) {
        i2cStart();                  /* START */
        i2cSend(CLOCK_ADDRESS_W);    /* MT_SLA */
        i2cSend(curr_time.hours);    /* DATA */
        i2cSend(curr_time.minutes);
        i2cSend(curr_time.seconds);
        i2cStop();                   /* STOP */

        tick();
        _delay_ms(5000);
    }

    return 0;
}
