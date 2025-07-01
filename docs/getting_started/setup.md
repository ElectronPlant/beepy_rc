# Setup Guide
This project uses the standard STM development environment with STM32CubeProgrammer, STM32CubeMX and STM32CubeIDE.

## Installation

To install the required components follow the guide below:

* [guide](https://medium.com/@erbo-engineering/using-vs-code-for-embedded-stm32-development-14405ed4ac82)

Summary of the overall process:

* Control version and terminal for windows:
    * [Git](https://git-scm.com/downloads) (for windows install git bash).
* Tool to program the STM32 microcontroller:
    * Download [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html)
    * Option 1: Add it to the path environment variable as: <install-path>\STM32Cube\STM32CubeProgrammer\bin.
    * Option 2: In the Makefile update the __STLINK_TOOLCHAIN_PATH__ symbol with your installation path.
* (optional) ST tool for MCU pin assignment and automatic code initialization
    * [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html).
    * It is better to maintain a separate project with with the cubeMX code to check the peripheral configurations and initialization procedure.
* Debugging on STM32 with GDB:
    * Install [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html). Note, this program is just needed to get the STLink GDB Server (there may be a better way to obtain it).
    * Once installed, the GDB Server path needs to be used in the launch.json (check below for further info): ```STM32CubeIDE_1.18.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.stlink-gdb-server.win32_2.2.100.202501151542\tools\bin\ST-LINK_gdbserver.exe```
* Cross compiler:
    * [gcc-arm-none-eabi](https://developer.arm.com/downloads/-/gnu-rm)
    * Option 1: Add it to the path environment variable as: <install-path>\Arm GNU Toolchain arm-none-eabi\14.2 rel1\bin.
    * Option 2: Modify the __ARM_TOOLCHAIN_PATH__ symbol in the makefile with the toolchain path.
* Makefile:
    * [GnuWin32](https://gnuwin32.sourceforge.net/packages/make.htm)

## Compile, Flash and Debug the Firmware
All the compilation is done using the Git Bash terminal, to maintain compatibility with other environments.

On VS code, on the terminal screen, the down arrow next to the '+' symbol should display the terminal options. Select Git Bash.

Navigate to the following directory:
```bash
cd firmware/make
```
Then to perform a clean compilation + flashing the MCU + monitoring the debug interface run:
```bash
make clean && make all && make flash && make monitor
```

Faster compilation can be done adding more jobs:
```bash
make all -j3
```

## Debug with GDB

### Setting up
Debugging with GDB enables you to stop the code execution at different points; run the code line by line; check the call stack at any point in the code; monitor variables and registers during the code excution; alongside other invaluable tools. This is a much better option that the infamous _printf debugging_ approach.

Setting VS code to support is relatively straight forward:

1. Install the [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) plugin in VS code.
2. Create the launch.json configuration file by:
    * On the VS code at the left of the screen, select the __Run & Debug__ tab.
    * If you do not have the _launch.json_ file created, there should be an option to create it. Otherwise open the launch.json file.
    * Copy the code from bellow adjusting the values within __<>__

```
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Cortex ST-Link",
            "cwd": "${workspaceRoot}",
            "executable": <1 - project.elf>,
            "request": "launch",
            "type": "cortex-debug",
            "servertype": "stlink",
            "stlinkPath": <2 - ST-LINK_gdbserver.exe>,
            "stm32cubeprogrammer": <3 - STM32CubeProgrammer>,
            "device": "STM32F446RET6",
            "svdFile": "./firmware/lib/mcu/stm32f446x/STM32F446.svd",
            "runToEntryPoint": "main",
            "showDevDebugOutput": true,
            "interface": "swd",
            "armToolchainPath": "Arm GNU Toolchain arm-none-eabi",
        },
    ]
}
```

Things to replace:

1. This is the relative path from the project's root to the __.elf__ file generated after compiling. For this project ```"./firmware/make/bin/beepy_rc.elf"```
2. This is the path to the ST-LINK_gdbserver.exe file, which should be installed as part of the CubeIDE installation. ```"<installation path>\bin\ST-LINK_gdbserver.exe"```
3. This is the path of the STM32CubeProgrammer's bin directory. ```"<install path>\\STMicroelectronics\\STM32Cube\\STM32CubeProgrammer\\bin"```
4. This is the path to the Arm tool chain's bin directory. ```<install path>\\Arm GNU Toolchain arm-none-eabi\\14.2 rel1\\bin"```

### Debug
To run create a GDB debugging session first ensure that the .elf file has been generated using the debug flags
```
CFLAGS += -g -gdwarf-2
```
Ensure that the board is connected through the ST-link, in the Nucleo board this is the USB in the board.

Then on VS code select the __Run and Debug__ tab on the left menu, followed by start debugging. Note that you should have selected the "Cortex ST-Link" configuration created earlier.
This should created the GDB session stopping the execution in the main function. From this point on it should work just as any other GDB session.