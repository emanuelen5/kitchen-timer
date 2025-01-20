# Schematic
## Crystal Oscilator
* We are using a ... crystal oscilator
* ATmega328p datasheet recommend caps of 2-22pF to go with it.
## Reset Button
* We are not adding a pull-up resistor because the RESET pin on the ATmega328p alrady has one.

## Bootloader auto-reset
* We will use the DTR line of the UART module to trigger a quick reset of the MCU. [[1]]

## Bootloader upload pins
[TODO]

## Power LED
[TODO]

## References:
* <a id="1">[1]</a> [https://rheingoldheavy.com/arduino-from-scratch-part-11-atmega328p-dtr-and-reset/]
