# MAX72xx Environment

## Purpose

Tests basic communication with the 16×16 LED matrix by toggling every pixel individually over SPI.

## Hardware Requirements

- 16×16 LED matrix driven by MAX72xx ICs via SPI (pins PB0, PB2, PB3, PB5)

## Expected Outcome

All 256 pixels light up one by one (left-to-right, top-to-bottom), and then turn off in the same order. The sequence repeats indefinitely, confirming that all pixels and the SPI link are working correctly.
