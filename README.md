# BeepyRC
Hobby project to make a RC rover platform using spare RC parts. The goal of this project is to develop a complete platform to support long term RC assisted missions.

Content summary

    * The platform is built on a custom PCB based on a STM32F4 microcontroller.
    * The firmware is implemented using FreeRTOS for simple and reliable multitasking and real-time operation.
    * The enclosure is fully 3d printable.
    * Support for off-the-shelf RC components with SBUS, 5V VTX power output, 3x standard servo outputs.

## Release 1 - [PoC] Nerf Blaster FPV Rover
Minimal-feature rover to support a simple, yet fun mission: exploring the house and firing a Nerf blaster in FPV (First-Person View).

The rover enables you to explore the house remotely using the RC interface and the FPV live feed provided by the video transmitter (VTX). The Nerf blaster and the VTX are mounted on a servo-operated arm, which controls their yaw and pitch angles. The arm provides two extra degrees of freedom to easily explore the room and aim the blaster. Once the target has been acquired, take the shot remotely using the aux channels of the RC remote.

![BeepyRc](./docs/imgs/beepyRC.jpg)

---

# Documentation
For more information about the project check the documentation in ```docs/``` or deplay with [mkdocs](https://www.mkdocs.org/)


## Installing MKDOCS
run:
```
pip install mkdocs
```
or:
```
python -m pip install mkdocs
```

### Install the Material theme:
```
pip install mkdocs-material
```
or:
```
python -m pip install mkdocs-material
```

### Install mkdoxy
```
pip install mkdoxy
```
or
```
python -m pip install mkdoxy
```

### Install Doxygen
The installation procedure depends on the operating system, see: [Doxygen docs](https://www.doxygen.nl/manual/install.html)

For ubuntu:
```
sudo apt-get install doxygen
```

### Deploy docs
```
mkdocs serve
```

then the documentation will be served at:
```
http://127.0.0.1:8000/
```
