# General Notes on the PCB Design

## Intro

The schematics and the PCB design are being done with Kicad.

The basic information about the requirements for the MCU are based on the following documents:

* [MCU datasheet](https://www.st.com/resource/en/datasheet/stm32f446re.pdf)
* [STM32 hardware guidelines](https://www.st.com/resource/en/application_note/an4488-getting-started-with-stm32f4xxxx-mcu-hardware-development-stmicroelectronics.pdf)
* [Clock appliction note](https://www.st.com/resource/en/application_note/cd00221665-oscillator-design-guide-for-stm8af-al-s-stm32-mcus-and-mpus-stmicroelectronics.pdf)


## References

Reference designs:

* [Simon Says](https://github.com/ElectronPlant/Simon_says_ATtiny/blob/master/Hardware/Simon_says_Attiny_Print_Schematics.pdf)
* [Flight controller](https://easyeda.com/editor#id=fbe44b42766e4ebb9b2bbff61543d910)
* [Flight controller 2](https://github.com/nppc/FF4Nano/blob/master/FF4NanoUSB.pdf)
* [Guide for fight controllers](https://flying-rabbit-fpv.com/2020/10/06/designing-my-own-flight-controller/)
* [STM32 example](https://stm32world.com/images/1/16/MCUSTM32F446_rev._a_schematics.svg)
* [Nucleo board schematic](https://www.st.com/resource/en/schematic_pack/mb1136-default-c03_schematic.pdf)


# Design notes

## MCU

### Debugging

The debugging will be done with the STLink V2 debugger. There is the STLink V3 version, but
this is not selected since it is not the one used by the Nucleo board (it may not be supported) by
the MCU.

A guide on debugging for STM32 can be seen [here](https://stm32-base.org/guides/connecting-your-debugger.html).
There are different pinouts that may be used. The PCB will be done using the Nucleo output.

There is also the debugging UART interface. This interface may be used for configurations or user level information.
This UART interface can be implemented using USB phy of the STM, using an external UART to USB adapter IC, or relying on an external UART to USB module. To minimize the development efforts and the component count on the PCB, the debugging UART interface will rely on the external UART to USB module (e.g. [see link](https://www.robotics-university.com/2018/04/usb-to-uart-ttl-bridge-cp2102-module.html)).

### Power, Grounding and Debugging

The MCU will be powered from a 3.3V rail.
The decoupling strategy is done following page 9 from [AN4488](https://www.st.com/resource/en/application_note/an4488-getting-started-with-stm32f4xxxx-mcu-hardware-development-stmicroelectronics.pdf).

* There is one 10uF capacitor for the MCU
* There is one additional 100nF cap for each VDD and VSS pair.
* Since there is no battery onboard, Vbat is connected to VDD with an additional 100nF cap.
* VDDA is connected to VDD through a ferrite bead.
* VDDA and VSSA are decoupled using a 1uF and a 100nF cap.
* GNDA is directly connected to GND.

### Clocks

There are two crystals used for the MCU. This approach is based on what exists on the Nucleo board.
The design and value calculation of the capacitor and external resistors is based on [AN2867](https://www.st.com/resource/en/application_note/cd00221665-oscillator-design-guide-for-stm8af-al-s-stm32-mcus-and-mpus-stmicroelectronics.pdf).

#### HSE

HSE, the high frequency clock is set at 8MHz, following the suggestions on keeping it as low as possible (reference pending).

Part: [HY8M49SSMDOB2R20](https://jlcpcb.com/partdetail/Huiyuancrystal-HY8M49SSMDOB2R20/C5265773)

#### LSE

LSE, the low frequency clock is set to 32.768kHz.

Part: [M332768DWNAC](https://www.lcsc.com/product-detail/C2838416.html)


## Power

There are two options to power the board, the external battery or the Debug USB.

The external battery (2s lipo battery from 6 to 12V). This source is current limited to 6A and has reverse voltage protection with the TVS and the current limiting PTC.
__Note__: there is no over-discharge protection built into the battery. This is intentional, since high current spikes will cause the battery voltage to sag and falsely trigger any protection. Instead, the MCU is responsible for monitoring battery voltage and disabling the regulated 5 V rail if necessary. This 5 V rail powers all components except the motors, which only run when explicitly controlled by the MCU. Once the 5 V rail is disabled, the board remains idle until the battery is disconnected for a few seconds and then reconnected.

5V input on the Debug USB. This source only supplies the core functionality (i.e. everything except for the motors, servos, VTX, external 3V3 rail).
The input is current limited to 500mA and reverse voltage protected with the OR diodes.

In case both sources are connected at the same time the OR diodes will select the higher voltage source.


### Distribution
The battery input is current-limited to 6A, supplying both the drive motors and the 5 V buck converter.
Of this 6 A:

* 2A are allocated for the drive motors.
* 4A are available for the 5V buck converter.

The 5 V buck converter powers:

* Servos and night lights: up to 2.6A.
* Video transmitter (VTX): up to 500mA.
* External 3.3 V source: up to 500mA, intended as a backup to power additional external sensors (e.g. GPS).
* Core functionality: 400mA, which may also be supplied from the debug USB 5V rail.

The core functionality 5V source powers:

* the RC module (up to 100mA).
* The remaining 300mA powers the 3.3 V regulator, which supplies the MCU and internal sensors.
