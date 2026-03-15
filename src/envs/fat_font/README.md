# Fat Font Environment

## Purpose

Tests the 16×16 LED matrix display by rendering digits using the "fat font" (6×7 pixels per character).

## Hardware Requirements

- 16×16 LED matrix driven by MAX72xx ICs via SPI (pins PB0, PB2, PB3, PB5)

## Expected Outcome

The LED matrix cycles through the digits 0–9, displaying each one for one second before moving to the next. The cycle repeats indefinitely.
