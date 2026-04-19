# BeepyRC
Hobby project to see if I can connect multiple spare parts I have around my house to make a radio
controlled rover.

## Summary

### Application Description
The following high-level plan will be followed:

* Everything will run on an STM32 nucleo board, refer to the [Hardware page](getting_started/hardware.md) for more information.
* FreeRTOS to schedule the different tasks.
* RX is received using a MX+ FrSky receiver, which is connected to the MCU using SBUS.
* 4 * Simple motors will be used to move the rover, which still need to be defined.
* VTx will be used too, yet to be defined.


### Repository Organization

The documentation (as required by MkDocs) is placed in the ```docs\``` directory.

The application is implemented in the ```firmware/``` directory, see the [doxygen files output](template/files.md) for further information about the internal organization.

### Getting started

For information on how to get started with the template, refer to the:

* [Hardware](getting_started/hardware.md) page for information on how to setup the module and connect the LEDs used by the application.
* [Setup](getting_started/setup.md) page for a summarized description of how to get the STM32 development environment in a windows PC.

## License

This project consists of multiple parts, each licensed differently:

1. FIRMWARE/CODE:
   This includes all files in ```/firmware``` unless specified otherwise.
   Licensed under the GNU General Public License v3.0.
   See ```/firmware/LICENSE``` for the full text.

2. HARDWARE (Schematics, PCB, Gerbers, BOMs, etc):
   This includes all files in ```/hardware``` unless specified otherwise.
   Licensed under the CERN Open Hardware License Version 2 - Strongly Reciprocal (CERN-OHL-S).
   See ```/hardware/LICENSE``` for the full text.

3. DOCUMENTATION (images, documentation, etc.):
   This includes all files in ```/docs``` unless specified otherwise.
   Licensed under Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0).
   See ```/docs/LICENSE``` for the full text.

### Credits
This projects uses external libraries that are documented in the ```NOTICE.md``` file in the
project's root directory.

More information on used external libraries an reference projects can be seen [credits](about/credits.md)
