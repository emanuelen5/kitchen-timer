# Sleep Environment

## Purpose

Tests the power-save sleep mode and the RTC (Timer2) 1-second interrupt used to wake the microcontroller.

## Hardware Requirements

- 32 kHz crystal connected to TOSC1/TOSC2 (for the Timer2 asynchronous clock)
- 3-LED binary counter connected to PC0–PC2

## Expected Outcome

The MCU enters power-save sleep and is woken up once per second by the RTC interrupt. On each wake cycle the LED counter increments (in the interrupt handler) and then decrements (back in the main loop), causing a visible oscillation on the LEDs that confirms the sleep/wake cycle is working correctly.
