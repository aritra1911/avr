# AVR Programs
These are my implementations of the example programs in the [Make: AVR Programming](https://www.oreilly.com/library/view/make-avr-programming/9781449356484/)
book. The AVR-Programming library was copied from the book's [repository](https://github.com/hexagon5un/AVR-Programming).

## My Setup
I'm currently using the ATmega328P from an Arduino UNO and a USBASP 2.0 for programming.

If you're facing an issue of `rc=-1` error from `avrdude` while programming the ATmega328P, head over to the discussion
thread of [this issue](https://github.com/hexagon5un/AVR-Programming/issues/24) of the book's repository.

# UART
The empty Arduino board is absolutely perfect for using as a serial interface. While a dedicated serial adapter would
look at the AVR from the computer's perspective, the Arduino will look at the computer from the AVR's perspective so
make sure you get the RX and TX pins hooked up right. TX refers to transfer to the computer so that hooks up to the TX
(pin 3) of the AVR. Similarly RX hooks up to RX (pin 2) of the AVR. No tricks here.
