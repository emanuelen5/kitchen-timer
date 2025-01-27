# Schematic
## Crystal Oscilator
* We are using a [TODO] crystal oscilator.
* ATmega328p datasheet recommend caps of 2-22pF to go with it.

## Reset Button
* We are not adding a pull-up resistor because the RESET pin on the ATmega328p alrady has one.

## Bootloader auto-reset
* We will use the DTR line of the UART module to trigger a quick reset of the MCU. [[1]](#1)

## Bootloader upload pins
* This is a set of pins to connect a programmer.

## Power LED
* We added an LED to indicate that the device is powered.

## UART Module
* We are using the BTE17-06B module
* This module has an overcurrent protection fusion that allows only 500mA.

## BMS Module
* Resistor R3 on the module is the charging current programming resistor. This resistor needs to be changed to an appropiate value for the battery used [TODO] (What value?) 

## Power Path
* See reference circuit in [[3]](#3)

## 3.3V Linear Regulator
* We are using the AP2111 [[2]](#2)
* The schematic has been taken from the datasheet (see "Typical Application", page 19)

## 5V Boost Converter
* We are using the TPS61200, Low Input Voltage Synchronous Boost Converter. [[4]](#4)
* The schematic has been taken from the [Sparkfun module](https://cdn.sparkfun.com/datasheets/Prototyping/Li_Power_Boost_Converter.pdf)

## LED Matrices Display
* We are using 4 common-cathode LED matrices.

## Piezo Speaker


* For the future, there are different circuits that could be implemented [[5]](#5)

## EMD protection
[TODO](Do we need this? We will have a USB connection. maybe we do.)

# References:
* <a id="1">[1]</a> [rheingoldheavy.com - Arduino from Scratch Part 11 – ATMEGA328P DTR and RESET](https://rheingoldheavy.com/arduino-from-scratch-part-11-atmega328p-dtr-and-reset/)
* <a id="2">[2]</a> [Hackaday - Lithium-Ion Batteries Power Your Devboards Easily](https://hackaday.com/2024/03/14/lithium-ion-batteries-power-your-devboards-easily/)
* <a id="3">[3]</a> [GitHub "TP4056-Power-Path-PCB"](https://github.com/DoImant/TP4056-Power-Path-PCB/blob/main/README.md)
* <a id="4">[4]</a> [Sparkfun - LiPower Boost Converter](https://www.sparkfun.com/lipower-boost-converter.html)

* <a id="5">[5]</a> [same sky - How to Increase the Audio Output of a Piezoelectric Transducer Buzzer](https://www.sameskydevices.com/blog/how-to-increase-the-audio-output-of-a-piezoelectric-transducer-buzzer)
