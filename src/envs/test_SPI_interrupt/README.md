# Test SPI Interrupt Environment

## Purpose

Tests interrupt-driven SPI transfers by repeatedly sending two bytes over the SPI bus.

## Hardware Requirements

- SPI bus (pins PB2, PB3, PB5) with chip select on PB1 (pin 9)
- 3-LED binary counter connected to PC0–PC2 (optional, used for visual feedback)

## Expected Outcome

Every second, two bytes (`0xAA`) are queued and transferred via SPI using the interrupt handler. A logic analyzer or oscilloscope on the SPI lines will confirm that the transfers are happening correctly at 1 Hz.
