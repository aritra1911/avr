#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include "binClock.h"
#include "sync.h"

volatile struct Time sync_time;
volatile uint8_t* t;
extern volatile struct Time curr_time;

ISR(TWI_vect) {
    /* Receive three bytes from Master as hours, minutes and seconds
       and update that as the current time */

    if (TWSR == TW_SR_SLA_ACK) {
        /* Receive hour byte next with ACK */
        t = &(sync_time.hours);

    } else if (TWSR == TW_SR_DATA_ACK) {
        /* Receive byte and send acknowledgement */
        *t++ = TWDR;

        if (t == &(sync_time.seconds)) {
            /* If about to receive last byte, receive and send NACK next */
            TWCR &= ~(1 << TWEA);
        }

    } else if (TWSR == TW_SR_DATA_NACK) {
        /* Final (seconds) byte received in TWDR, and NACK was sent */
        *t = TWDR;
        curr_time = sync_time;

        /* Reset TWEA for acknowledgement of next SLA */
        TWCR |= (1 << TWEA);

    } else if (TWSR == TW_ST_SLA_ACK) {
        /* On Read request, send a different byte next */
        TWDR = sync_time.minutes;

        /* We should be sending a single byte, so expect NACK from Master */
        TWCR &= ~(1 << TWEA);

    } else if (TWSR == TW_ST_DATA_NACK) {
        /* Data was sent and NACK was received from Master (as expected)
           Now clear TWEA for acknowledgement of next SLA */
        TWCR |= (1 << TWEA);
    }

    /* Clear I2C Interrupt Flag */
    TWCR |= (1 << TWINT);
}

void initI2CSlave(const uint8_t addr) {
    TWBR = 32;                          /* set bit rate */
               /* 8 MHz / (16 + 2 * TWBR * 1) = 100 kHz */

    /* Set Slave Address */
    TWAR = addr << 1;

    /* Enable I2C *//* Clear I2C Interrupt Flag *//* Enable I2C Interrupt */
    TWCR |= (1 << TWEN) | (1 << TWINT) | (1 << TWIE);
    /* Note: TWINT is cleared by writing logic 1 to it */

    /* Clear Start & Stop Condition bits */
    TWCR &= ~(1 << TWSTA) & ~(1 << TWSTO);
}

void connectI2CSlave(void) {
    /* Set TWEA for acknowledging when addressed */
    TWCR |= (1 << TWEA);
}

void disconnectI2CSlave(void) {
    /* Clear TWEA for ignoring (or sending NACK) when addressed.
       This is helpful to temporarily disconnect slave from I2C bus */
    TWCR &= ~(1 << TWEA);
}
