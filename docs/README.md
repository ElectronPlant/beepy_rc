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

### Source code

For the licensing information about the source code and the external libraries refer to the ```NOTICE.MD``` and ```LICENSE``` files in the project's root directly.

### Documentation
Licensing information for the documentation is in the [license](license.md) page.
