# BeepyRC
Hobby project to see if I can connect multiple spare parts I have around my house to make a radio
controlled rover. This is a project I'm doing for fun in my spare time, so there are no warranties
of operation or delivery.

## Project Roadmap
* [ ] __Release 1: Minimum Viable Rover__
    * [x] Receive and parse the RC frames.
    * [x] Use RC inputs to control motors.
    * [x] Custom hardware for the RC rover.
    * [ ] Control servos with aux channels (camera pan/tilt, fire nerf gun).
    * [ ] 3D printed case.
* [ ] __Release 2: Sensor-Assisted Rover__
    * [ ] Motor encoders for precise individual motor adjustments.
    * [ ] Accelerometer, gyroscope and barometer integration speed and rotation RC setpoints.
* [ ] __Release 3: Waypoint Mission Rover__
    * [ ] Odometry + Kinematic movement estimation.
    * [ ] GPS integration for accurate rover position.
    * [ ] Support for autonomous waypoint missions.
* [ ] __Release 4: Autonomous Robotic Platform__


## Summary

The following high-level plan will be followed:

* Everything will start on a STM32 Nucleo-F446RE board. Then, it will progress to a custom board. Refer to the [Hardware page](getting_started/hardware.md) for more information.
* The task scheduling will be done using [FreeRTOS](https://www.freertos.org/).
* Radio control setpoints will be received using a MX+ FrSky receiver, which is connected to the MCU through SBUS.
* 4 * Simple motors will be used to move the rover.
* VTx will be used too, yet to be defined.

---

## Releases

### Firmware Releases
No releases yet.

### Hardware

#### BeepyRcBrd Rev0.0

Initial release of the four wheel drive rover with preliminary support for all planned features.

* [Schematics](beepyRcBrd/rev_0_0/beepyRcBrd_rev_0_0.pdf)
* [Reworks](beepyRcBrd/rev_0_0/reworks.md)
* [Fabrication Files](beepyRcBrd/rev_0_0/fabrication_files.zip)

Checkout the [design notes](beepyRcBrd/pcb_notes.md).

### Mechanical

No releases yet.

---

## Getting started

For information on how to get started with the template, refer to the:

* [Hardware](getting_started/hardware.md) page for information on how to setup the module and connect the LEDs used by the application.
* [Setup](getting_started/setup.md) page for a summarized description of how to get the STM32 development environment in a windows PC.

### Repository Organization

The documentation (as required by MkDocs) is placed in the ```docs\``` directory.

The application is implemented in the ```firmware/``` directory, see the [doxygen files output](template/files.md) for further information about the internal organization.

The design files for the custom PCB are located in the ```hardware/``` directory.

The 3D printed case is located in the ```mechanical/``` directory.

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

## Credits
This projects uses external libraries that are documented in the ```NOTICE.md``` file in the
project's root directory.

More information on used external libraries an reference projects can be seen [credits](about/credits.md)
