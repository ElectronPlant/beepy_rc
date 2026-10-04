# BeepyRC
Hobby project to make a RC rover platform using spare RC parts. The goal of this project is to develop a complete platform to support long term RC assisted missions.

## Project Roadmap
* [ ] __Release 1: PoC Nerf Blaster FPV Rover__
    * [x] Receive and parse the RC frames.
    * [x] Use RC inputs to control motors.
    * [x] Custom hardware for the RC rover.
    * [X] Control servos with aux channels (camera pan/tilt, fire nerf gun).
    * [X] 3D printed case.
* [ ] __Release 2: Sensor-Assisted Rover__
    * [ ] Motor encoders for precise individual motor adjustments.
    * [ ] Accelerometer, gyroscope and barometer integration speed and rotation RC setpoints.
* [ ] __Release 3: Waypoint Mission Rover__
    * [ ] Odometry + Kinematic movement estimation.
    * [ ] GPS integration for accurate rover position.
    * [ ] Support for autonomous waypoint missions.
* [ ] __Release 4: Autonomous Robotic Platform__


---

# Releases

Release table linking releases with the supported versions and tags.

| Release     | FW Version | HW Version        | Mech Version |
|-------------|------------|-------------------|--------------|
| __Rel. 1__  | FWv0.0     | beepyRcBrd_rev0.0 | TODO         |


# Versions

Each component has its own versioning which are independent from the releases. This allows reusing a given version on multiple releases, or having multiple versions compatible between releases for patches and improvements.

## Firmware Versions

| Version     | Tag        | Description|
|-------------|------------|------------|
| __V 0.0__ | [FWv0.0](https://github.com/ElectronPlant/beepy_rc/releases/tag/FWv0.0)    | Minimal features for Release 1 |


## Hardware

| Version | Tag | Schematics | Reworks | Fabrication Files| Description |
|---|---|---|---|---|---|
| __REV 0.0__ | [beepyRcBrd_rev0.0](https://github.com/ElectronPlant/beepy_rc/tree/beepyRcBrd_rev0.0)| [Schematics](beepyRcBrd/rev_0_0/beepyRcBrd_rev_0_0.pdf) | [Reworks](beepyRcBrd/rev_0_0/reworks.md) |[Fabrication Files](beepyRcBrd/rev_0_0/fabrication_files.zip)| Preliminary support for all planned features. Checkout the [design notes](beepyRcBrd/rev_0_0/design_notes.md). |


## Mechanical

| Version | Tag | STL Files| Description |
|---|---|---|---|
| __REV 0.0__ | [mech_rev0.0](https://github.com/ElectronPlant/beepy_rc/releases/tag/Mech_rev0.0)| [STL](mech/rev_0_0/BeepyRcMech_rev_0_0.zip) | See [Docs](mech/mech_docs.md) |

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


## Credits
This projects uses external libraries that are documented in the ```NOTICE.md``` file in the
project's root directory.

More information on used external libraries an reference projects can be seen [credits](about/credits.md)
