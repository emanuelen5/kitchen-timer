# Kitchen timer

We're building a Kitchen timer!
The project is heavily inspired by [REST: Kitchen timer by mkdxdx [Hackaday.io]](https://hackaday.io/project/194386-rest-kitchen-timer).

## Introduction:
Our kitchen timer will be an iteration of [mkdxdx](https://hackaday.io/mkdxdx) project "REST: Kitchen timerREST: Kitchen timer". We will keep the "dial" (rotate-to-set) concept and the 16x16 led matrix but we want to do it with an ATmega328p and a custom PCB.

Among other features that we would like to add are:
* The possibility to set up several timers (in case you need to keep track of more than one pot when cooking dinner)
* A UART communication port that would allow us to test the device via serial commands.

## Motivation:
Both [Erasmus Cedernaes](https://github.com/emanuelen5) and [Nicolas Perozzi](https://github.com/nperozzi) belong to [Stockholm Makerspace](https://www.makerspace.se/). On the same day, we messaged each other a link to mkdxdx​ project. We were very excited about building something that would be "practical" so we decided to team up and work on this together. Additionally, we share an interest in working with the AVR MCUs. This project is a nice blend of programming, electrical, mechanical, 3d printing, and design but, without feeling overwhelming.

## Product Requirements:
* It needs to allow for several timers to run in parallel.
* Powered by battery
* Charged by USB-C.
* Programmable.
* It needs to be possible to stick to the kitchen fridge.

## Hardware

* Atmega328P as microcontroller
* Rotary encoder for setting the time
* 16x16 LED matrix as display
* 32 kHz crystal for accurate timekeeping
* (Passive?) buzzer for beeping

### Schematic

Prototyping setup on the breadboard:

![Breadboard setup](./fritzing/schema_bb.svg)

The the schematic:

![Schematic](hardware\kitchen_timer-hardware.kicad_pro)

## Software

The software is divided into several "environments", similar to "sketches" in Arduino. They are used to test different aspects of the hardware individually, to make sure that the assembled hardware works as intended.

The environments are located in the [`src/envs`](./src/envs/) folder.

### Programming

To be able to program the microcontroller, you need `avrdude` and `pio` (the PlatformIO command line tool).

* Avrdude 7.3 can be downloaded from <https://github.com/avrdudes/avrdude/releases/tag/v7.3>. Make sure to add it to your `PATH`.
* `pio` can be downloaded from <https://pypi.org/project/platformio/> or installed using `pip` (the official Package Installer for Python)

Then run

```bash
./flash.sh <programmer> <environment>
```

to build and program your board with the selected environment.
