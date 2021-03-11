/*
   Setup: AVR I2C Slave with an ESP32 as master

   Description : Master (ESP32) may throw a byte at Slave (AVR) and AVR acknowledges the reception. AVR is interrupted
                 and then updates its local copy (variable `k'). At the same time AVR continually prints the value of
                 `k' via UART.

   Note: - TWI has been referred to as I2C (and pronounced "I Squared C") throughout this program
         - Disconnecting Master from the I2C bus doesn't affect working of the Slave, i.e. AVR keeps UARTing `k'
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>
#include "USART.h"

#define I2C_SLAVE_ADDRESS 0x54

ISR(TWI_Vect);
void initI2CSlave(const uint8_t);
void connectI2CSlave(void);
void disconnectI2CSlave(void);

volatile uint8_t k;

ISR(TWI_vect) {
    /* On data receive with acknowledge, received data is in TWDR
       Update k with this new value of TWDR */
    if (TWSR == TW_SR_DATA_ACK)
        k = TWDR;

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

int main(void) {
    initUSART();
    initI2CSlave(I2C_SLAVE_ADDRESS);

    k = 0;

    connectI2CSlave();  /* Enable ACK when addressed */
    sei();

    while (1) {
        printString("k = ");
        printByte(k);
        transmitByte('\n');
        _delay_ms(500);
    }

    return 0;
}
