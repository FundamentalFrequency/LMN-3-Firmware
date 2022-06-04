# LMN-3-Firmware
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

This repository contains the Teensy 4.1 firmware that sends MIDI data to the [LMN-3-DAW](https://github.com/FundamentalFrequency/LMN-3-DAW).

If you would like to support the project, please consider becoming a [sponsor](https://github.com/sponsors/stonepreston).

## Building

This project uses [PlatformIO](https://platformio.org/) as its build system. Please refer to the [documentation](https://docs.platformio.org/en/latest/) for instructions on how to get started if you are not familiar with it. This will generally amount to installing the PlatformIO plugin for your IDE and then using the PlatformIO plugin GUI to build and push the firmware to the Teensy via USB, similar to the way the Arduino IDE works. The `platformio.ini` file contains important configuration used by platformIO at build time. 

## Thanks
This project would not be possible without the following free and open source projects:

- [Control Surface](https://github.com/tttapa/Control-Surface)
- [PlatformIO](https://github.com/platformio)

[Control Surface](https://github.com/tttapa/Control-Surface) is the single greatest Arduino library I have ever seen. The amount of work that has gone into it is astounding. It is well tested, well documented, and a general pleasure to use in a world where most Arduino libraries leave you pulling your hair out. It has simplified what would be an extremely complex project and allowed the LMN-3 firmware to exist as a single source file. It is greatly appreciated.

