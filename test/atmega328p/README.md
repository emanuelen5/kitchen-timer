# Unit-testing on ATmega328P

This sub-directory contains tests that run on the ATmega328P natively.

## Hook up

To run the unit-tests, you will need to hook up a serial port to the microcontroller.

## Run the tests

To run it, you can use any of the `atmega328p` environments (see the [platformio.ini](../../platformio.ini) file). E.g.

```bash
pio test -e timer
```
