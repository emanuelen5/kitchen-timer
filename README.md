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

### Enclosure

The .stl files are available in [./enclosure](./enclosure/) so you can 3D-print the enclosure yourself.

## Software

You can flash the program directly using the ISP header on the board if you want (the same procedure as flashing the bootloader, below). But it's more convenient to add the bootloader so you don't have to open up the enclosure for each software update. Then you can instead program it over the USB-C connector.

For that, you need the bootloader firmware and programmer software which you can download from <https://github.com/emanuelen5/kitchen-timer/releases/tag/bootloader-v1.2.0>.

### Bootloader

To flash the microcontroller with the bootloader, you need `avrdude`.
Avrdude 7.3 can be downloaded from <https://github.com/avrdudes/avrdude/releases/tag/v7.3>. Make sure to add it to your `PATH`.

Connect your programmer of choice to the 6-pin in-system programmer (ISP) connector: \
![6-pin ISP header, as viewed from the top](./docs/isp.svg)
<!--
Attribution:
["6- and 10-pin AVR ISP headers"](https://en.wikipedia.org/wiki/In-system_programming#/media/File:Isp_headers.svg)
by osiixy (2013-01-05), used under [CC BY 3.0](https://creativecommons.org/licenses/by/3.0/).
Adaptations: removed 10-pin header and some text; improved the contrast for dark
             mode.
-->

Then run the following command (replace `<programmer>` with your programmer of choice, like `usbtiny`):

```bash
# the bootloader.hex file needs to be in the current directory
avrdude -p atmega328p -c <programmer> -U flash:w:bootloader.hex:i \
  -U lfuse:w:0x62:m -U hfuse:w:0xD0:m -U efuse:w:0xFF:m
```

> [!TIP]
> Use `avrdude -c ?` to see which programmers are available


This flashes the bootloader and sets the required fuses for the bootloader.

After that you can disconnect the programmer and assemble the device.

### Programmer

Now that you have the bootloader on the device, you can use the USB-C cable for programming it by using the programmer application.

#### Dependencies

First you will need to install some packages that the programmer needs (files from <https://github.com/emanuelen5/kitchen-timer/releases/tag/bootloader-v1.2.0>).

```bash
pip install -r ./programmer/requirements.txt
```

You can now program the application onto the device through the bootloader!

### Upload firmware

Download the latest revision of the firmware (if you want to build it yourself, see [#Development](#development) below) from the releases <https://github.com/emanuelen5/kitchen-timer/releases>.

Connect the USB cable to the device, and then run

```bash
python3 ./programmer/programmer.py --hexfile firmware.hex
```

## Development

### Firmware

To build the firmware, you need `pio` which can be downloaded from <https://pypi.org/project/platformio/> or installed using `pip` (the official Package Installer for Python)

To build any of the environments, use the `pio` command, like

```bash
# builds the main application and puts it in .pio/builds/application/firmware.hex
pio run -e application
```

Then use the programmer to upload the firmware to the device.

## Background

Our kitchen timer is an iteration of [mkdxdx](https://hackaday.io/mkdxdx) project "REST: Kitchen timerREST: Kitchen timer". We wanted to keep the "dial" (rotate-to-set) concept and the 16x16 led matrix but do it with an ATmega328p and a custom PCB.
In the end we scrapped the dial since the design became too complicated, and settled on a simpler design.

### Motivation

Both [Erasmus Cedernaes](https://github.com/emanuelen5) and [Nicolas Perozzi](https://github.com/nperozzi) belong to [Stockholm Makerspace](https://www.makerspace.se/). On the same day, we messaged each other a link to mkdxdx​ project. We were very excited about building something that would be "practical" so we decided to team up and work on this together. Additionally, we share an interest in working with the AVR MCUs. This project is a nice blend of programming, electrical, mechanical, 3d printing, and design but, without feeling overwhelming.
