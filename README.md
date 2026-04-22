# Kitchen timer

We're building a Kitchen timer!
The project is heavily inspired by [REST: Kitchen timer by mkdxdx [Hackaday.io]](https://hackaday.io/project/194386-rest-kitchen-timer).

## Features

* ⏱️ Allows several timers to run in parallel
* ⚡ Is battery powered
* 🔌 Charged by USB-C
* 💻 Programmable through the same USB port
* 🐍 Easter eggs

## Hardware

* Atmega328P as microcontroller
* Rotary encoder for setting the time
* 16x16 LED matrix as display
* 32 kHz crystal for accurate timekeeping
* Passive buzzer for beeping

### Manufacturing / gerber files

If you want to build the PCB yourself, you can download the latest release of Gerber files from Github: <https://github.com/emanuelen5/kitchen-timer/releases/latest>

## Software

### Bootloader

You need a bootloader on the microcontroller to be able to program it through the USB connector.

To flash the microcontroller with the bootloader, you need `avrdude` and `pio` (the PlatformIO command line tool).

* Avrdude 7.3 can be downloaded from <https://github.com/avrdudes/avrdude/releases/tag/v7.3>. Make sure to add it to your `PATH`.
* `pio` can be downloaded from <https://pypi.org/project/platformio/> or installed using `pip` (the official Package Installer for Python)

Connect your programmer of choice to the 6-pin in-system programmer (ISP) connector: \
![6-pin ISP header, as viewed from the top](./docs/isp.svg)
<!--
Attribution:
["6- and 10-pin AVR ISP headers"](https://en.wikipedia.org/wiki/In-system_programming#/media/File:Isp_headers.svg)
by osiixy (2013-01-05), used under [CC BY 3.0](https://creativecommons.org/licenses/by/3.0/).
Adaptations: removed 10-pin header and some text; improved the contrast for dark
             mode.
-->

Then run

```bash
./flash.sh <programmer> bootloader
```

to flash the bootloader and set the fuses (to `lfuse=0x62 hfuse=0xD0 efuse=0xFF`).

### Application

First you will need to install some packages that the programmer needs, and build the application.

```bash
pip install -r ./programmer/requirements.txt
pio run -e application
```

You can now program the application onto the device using the bootloader!

Connect the USB cable to the device, and then run

```bash
python3 ./programmer/programmer.py --hexfile .pio/build/application/firmware.hex
```

## Background

Our kitchen timer is an iteration of [mkdxdx](https://hackaday.io/mkdxdx) project "REST: Kitchen timerREST: Kitchen timer". We wanted to keep the "dial" (rotate-to-set) concept and the 16x16 led matrix but do it with an ATmega328p and a custom PCB.
In the end we scrapped the dial since the design became too complicated, and settled on a simpler design.

### Motivation

Both [Erasmus Cedernaes](https://github.com/emanuelen5) and [Nicolas Perozzi](https://github.com/nperozzi) belong to [Stockholm Makerspace](https://www.makerspace.se/). On the same day, we messaged each other a link to mkdxdx​ project. We were very excited about building something that would be "practical" so we decided to team up and work on this together. Additionally, we share an interest in working with the AVR MCUs. This project is a nice blend of programming, electrical, mechanical, 3d printing, and design but, without feeling overwhelming.
