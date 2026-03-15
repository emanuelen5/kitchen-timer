# Application Environment

## Purpose

Tests the full application layer with all hardware components working together: the LED matrix display, rotary encoder, push button, RTC, buzzer, and UART serial interface.

## Hardware Requirements

- 16×16 LED matrix driven by MAX72xx ICs via SPI (pins PB0, PB2, PB3, PB5)
- Rotary encoder connected to PD2 (channel A) and PD3 (channel B)
- Encoder push button connected to PD4 (SW_PIN)
- 32 kHz crystal connected to TOSC1/TOSC2 (for 1 Hz RTC tick)
- Passive buzzer connected to the buzzer output pin
- 3-LED binary counter connected to PC0–PC2
- UART serial port

## Expected Outcome

The kitchen timer application runs fully:
- Rotating the encoder sets the timer duration.
- A single button press starts or stops the active timer.
- A double press creates a new timer.
- A long press deletes the current timer.
- The LED matrix renders the timer state and counts down in real time.
- The buzzer sounds when a timer expires.
- Serial commands can be sent over UART to control and inspect the application.
