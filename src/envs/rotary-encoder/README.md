# Rotary Encoder Environment

## Purpose

Tests the rotary encoder and its push button by reflecting the input on the LED counter.

## Hardware Requirements

- Rotary encoder connected to PD2 (channel A) and PD3 (channel B)
- Encoder push button connected to PD4 (SW_PIN)
- 3-LED binary counter connected to PC0–PC2

## Expected Outcome

- Rotating the encoder **clockwise** increments the LED counter.
- Rotating the encoder **counter-clockwise** decrements the LED counter.
- A **single press**, **double press**, or **long press** of the button resets the counter to 0.
