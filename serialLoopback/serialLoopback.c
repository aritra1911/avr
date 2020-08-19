#include <avr/io.h>
#include "pinDefines.h"
#include "USART.h"

int main(void) {
    char serialCharacter;

    LED_DDR = 0xff;  // Set all pins for output
    initUSART();
    printString("Hello from AVR uC\r\n");

    while(1) {
        serialCharacter = receiveByte();
        transmitByte(serialCharacter);
        LED_PORT = serialCharacter;
    }

    return 0;
}
