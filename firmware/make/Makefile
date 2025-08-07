###############################################################################
# BeepyRC Firmware Makefile.
###############################################################################
#
# Created by: David Arnaiz
#
# Usage:
#    -) Compile all:
#           $ make all
#   -) Upload binary to the MCU using the STLink:
#           $ make flash
#   -) Clean:
#           $ make clean
#   -) Display SWD logs:
#           $ make monitor
#   -) Print help function:
#           $ make help
###############################################################################


###############################################################################
# OPTIONS
###############################################################################

# Compile in debug mode
DEBUG        ?= 1
# Compile and run tests
TEST         ?=

# Compile and upload test
RUN          ?= false

# Generate .map file with memory information
GENERATE_MAP_FILE ?= true


###############################################################################
# TARGET INFORMATION
###############################################################################
PROJECTNAME = beepy_rc

# Microcontroller information
TARGET := STM32F44RETx
CPU    := cortex-m4
FPU    := fpv4-sp-d16
FLOAT-ABI := hard # Specifies whether to use hardware or software FPU implementations.
BOARD := nucleo


###############################################################################
# PATHS
###############################################################################
# Build paths options (this directories will be deleted with the clean, so the need to be subdirectories).
BUILD_DIR  := build
OUTPUT_DIR := bin

# Toolchain paths - <USER> This needs to be adjusted
#  1. Need to end in "/".
#  2. It can be left empty if the toolchain is in the path environment variable.
ARM_TOOLCHAIN_PATH    := $(HOME)/.setup/arm/
STLINK_TOOLCHAIN_PATH := $(HOME)/.setup/stm/cube_programmer/

# Final path definition
OBJ_DIR := $(BUILD_DIR)
BIN_DIR := $(OUTPUT_DIR)


###############################################################################
# DEFINES
###############################################################################
AS_DEFS :=

#C_DEFS :=  \ # // TODO check if these are needed or not
#-DUSE_FULL_LL_DRIVER \
#-DSTM32F303xC \
#-DHSE_VALUE=8000000 \
#-DHSE_STARTUP_TIMEOUT=100 \
#-DLSE_STARTUP_TIMEOUT=5000 \
#-DLSE_VALUE=32768 \
#-DEXTERNAL_CLOCK_VALUE=8000000 \
#-DHSI_VALUE=8000000 \
#-DLSI_VALUE=40000 \
#-DVDD_VALUE=3300 \
#-DPREFETCH_ENABLE=1
# C defines
#-DUSE_HAL_DRIVER --> Check if this can be removed now
C_DEFS =  \
-DUSE_FULL_LL_DRIVER \
-DSTM32F446xx

# DEFS
#  - port used to program the device, SWD = ST-LINK.
#  - CLK frequency in kHz used for the connection.
STLINK_DEFS := \
port=SWD \
freq=4000

# Defines for tests
TEST_DEFS   := -DTEST_$(TEST)
ifeq ($(TEST), 1)
C_DEFS += TEST_DEFS
endif

###############################################################################
# FLAGS
###############################################################################
# ARM Tool chain
ARM_TOOL_PREFIX := arm-none-eabi-
CC = $(ARM_TOOLCHAIN_PATH)$(ARM_TOOL_PREFIX)gcc
AS = $(ARM_TOOLCHAIN_PATH)$(ARM_TOOL_PREFIX)gcc -x assembler-with-cpp
CP = $(ARM_TOOLCHAIN_PATH)$(ARM_TOOL_PREFIX)objcopy
SZ = $(ARM_TOOLCHAIN_PATH)$(ARM_TOOL_PREFIX)size

HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

# STLINK Tool chain
STLINK_TOOL_PREFIX := STM32_Programmer_CLI
STLINK_TOOL := $(STLINK_TOOLCHAIN_PATH)$(STLINK_TOOL_PREFIX)

# Cfgs
OPTIMIZATION := -Og # // TODO This may need to be adjusted in the future
MCU          := -mcpu=$(CPU) -mthumb -mfpu=$(FPU) -mfloat-abi=$(FLOAT-ABI)

# Generate dependency information
DEPS_FLAGS = -MMD -MP -MF"$(@:%.o=%.d)"

# Listing file generation
LST_FLAGS = -Wa,-a,-ad,-alms=$(OBJ_DIR)/$(notdir $(<:.c=.lst))

# Main flags
TRAIL_FLAGS := -Wall -fdata-sections -ffunction-sections
CFLAGS    = $(OPTIMIZATION) $(MCU) $(TRAIL_FLAGS) $(DEPS_FLAGS) $(LST_FLAGS)
ASFLAGS   = $(OPTIMIZATION) $(MCU) $(TRAIL_FLAGS) $(DEPS_FLAGS)
ASMMFLAGS = $(OPTIMIZATION) $(MCU) $(TRAIL_FLAGS) $(DEPS_FLAGS)


# Debug
ifeq ($(DEBUG), 1)
	CFLAGS += -g -gdwarf-2
endif

#
# Linker flags
# link script
LDSCRIPT := ../external_libs/mcu/stm32f446x/STM32F446XX_FLASH.ld
# libraries
LIBS    := -lc -lm -lnosys
LIBDIR  :=

LDFLAGS := $(MCU) -specs=nano.specs -Wl,-Map=$(OBJ_DIR)/$(PROJECTNAME).map,--cref -Wl,--gc-sections


###############################################################################
# SOURCES
###############################################################################
# STM drivers
C_STM_DRIVERS := \
../external_libs/mcu/stm32f446x/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_gpio.c \
../external_libs/mcu/stm32f446x/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usart.c \
../external_libs/mcu/stm32f446x/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_dma.c \
../external_libs/mcu/stm32f446x/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rcc.c \
../external_libs/mcu/stm32f446x/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_utils.c \
../external_libs/mcu/stm32f446x/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_exti.c \
../external_libs/mcu/stm32f446x/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_tim.c \
../external_libs/mcu/stm32f446x/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
../external_libs/mcu/stm32f446x/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c

H_STM_DRIVERS := \
-I../external_libs/mcu/stm32f446x/STM32F4xx_HAL_Driver/Inc \
-I../external_libs/mcu/STM32F4xx_HAL_Driver/Inc/Legacy \
-I../external_libs/mcu/stm32f446x/CMSIS/Device/ST/STM32F4xx/Include

# CMSIS DRIVERS
C_CMSIS_DRIVERS :=
H_CMSIS_DRIVERS := \
-I../external_libs/mcu/stm32f446x/CMSIS/Include \
-I../external_libs/mcu/stm32f446x/CMSIS/Core/Include

# STM intermediate files
#	@brief Files that are generated by STM, but not part of the drivers.
C_STM_INTERMEDIATE := \
../app/Src/stm32f4xx_it.c \
../app/system/syscalls.c \
../app/system/sysmem.c \
../app/system/system_stm32f4xx.c

H_STM_INTERMEDIATE := \
-I../app/system

# FreeRTOS files
C_FREERTOS := \
../external_libs/FreeRTOS/croutine.c \
../external_libs/FreeRTOS/event_groups.c \
../external_libs/FreeRTOS/list.c \
../external_libs/FreeRTOS/queue.c \
../external_libs/FreeRTOS/stream_buffer.c \
../external_libs/FreeRTOS/tasks.c \
../external_libs/FreeRTOS/timers.c \
../external_libs/FreeRTOS/portable/MemMang/heap_2.c

H_FREERTOS := \
-I../external_libs/FreeRTOS/include \
-I../external_libs/FreeRTOS/portable/GCC/ARM_CM4F

ifeq ($(CPU), cortex-m4) # If the CPU is not support there will be a compilation error
	C_FREERTOS += \
	../external_libs/FreeRTOS/portable/GCC/ARM_CM4F/port.c
	H_FREERTOS += \
	-I../external_libs/FreeRTOS/portable/GCC/ARM_CM4F
else
	$(error CPU not supported, check FreeRTOS ports)
endif

# BSP
ifeq ($(BOARD), nucleo)
	C_BSP :=
	H_BSP := -I../target/nucleo
else
	$(error BOART not supported, check targets)
endif

# Application files
C_APP_SOURCE := \
../app/main.c \
../app/rc/rc.c \
../app/rc/sbus.c \
../app/rc/serial.c \
../app/bsp/bsp.c \
../app/motion/motion.c \
../app/motion/drive_pwm.c \

H_APP_SOURCE := \
-I../app/rc \
-I../app/bsp \
-I../app/motion \
-I../configs

# Platform
C_PLATFORM := \
../platform/plt_assert.c

H_PLATFORM := \
-I../platform

# ASM sources
ASM_SOURCES := \
../external_libs/mcu/stm32f446x/startup_stm32f446xx.s
AS_INCLUDES :=

# ASM sources //TODO -> Check if this is needed or not
ASMM_SOURCES  :=
ASMM_INCLUDES :=

C_SOURCES  := $(C_STM_DRIVERS) $(C_CMSIS_DRIVERS) $(C_STM_INTERMEDIATE) $(C_FREERTOS) $(C_BSP) $(C_APP_SOURCE) $(C_PLATFORM)
C_INCLUDES := $(H_STM_DRIVERS) $(H_CMSIS_DRIVERS) $(H_STM_INTERMEDIATE) $(H_FREERTOS) $(H_BSP) $(H_APP_SOURCE) $(H_PLATFORM)

# libraries
LIBS	:= -lc -lm -lnosys
LIBDIR	:=

LD_SOURCES := -T$(LDSCRIPT) $(LIBDIR) $(LIBS)


###############################################################################
# RULES
###############################################################################
# default rule: build all
all: $(BIN_DIR)/$(PROJECTNAME).elf $(BIN_DIR)/$(PROJECTNAME).hex $(BIN_DIR)/$(PROJECTNAME).bin

########################################
# Building recepies
########################################
# list of objects
OBJECTS = $(addprefix $(OBJ_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(OBJ_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
OBJECTS += $(addprefix $(OBJ_DIR)/,$(notdir $(ASMM_SOURCES:.S=.o)))
vpath %.S $(sort $(dir $(ASMM_SOURCES)))

# Compile
$(OBJ_DIR)/%.o: %.c Makefile | $(OBJ_DIR) $(BIN_DIR)
	$(CC) -c $(C_DEFS) $(C_INCLUDES) $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.s Makefile | $(OBJ_DIR) $(OBJ_DIR) $(BIN_DIR)
	$(AS) -c $(AS_DEFS) $(AS_INCLUDES) $(ASFLAGS) $< -o $@
$(OBJ_DIR)/%.o: %.S Makefile | $(OBJ_DIR) $(OBJ_DIR) $(BIN_DIR)
	$(AS) -c $(ASMM_INCLUDES) $(ASMMFLAGS) $< -o $@

# Link
ELF_FILE_NAME := $(BIN_DIR)/$(PROJECTNAME).elf
$(ELF_FILE_NAME): $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LD_SOURCES) $(LDFLAGS) -o $@
	$(SZ) $@

# Generate output files
$(BIN_DIR)/$(PROJECTNAME).hex: $(ELF_FILE_NAME) | $(BIN_DIR)
	$(HEX) $< $@

$(BIN_DIR)/$(PROJECTNAME).bin: $(ELF_FILE_NAME) | $(BIN_DIR)
	$(BIN) $< $@

# Create directories
$(OBJ_DIR):
	mkdir $@
$(BIN_DIR):
	mkdir $@

-include $(wildcard $(OBJ_DIR)/*.d)

########################################
# Additional rules
########################################
# Delete all compilation files
clean:
	@echo "Cleaning up..."
	@if [ -d $(OBJ_DIR) ]; then rm -fR $(OBJ_DIR); fi
	@if [ -d $(BIN_DIR)   ]; then rm -fR $(BIN_DIR); fi
	@echo "Cleaned!"

# Load binary into the microcontroller
#		-v verify, -rst software reset.
flash:
	@echo "Programming..."
	@if [ ! -f "$(BIN_DIR)/$(PROJECTNAME).hex" ]; then \
		echo "The binary $(BIN_DIR)/$(PROJECTNAME).hex was not found"; \
	else \
		$(STLINK_TOOL) -c $(STLINK_DEFS) -w $(BIN_DIR)/$(PROJECTNAME).hex -v -rst; \
	fi

# Load binary into the microcontroller
#		-v verify, -rst software reset.
build_flash: $(BIN_DIR)/$(PROJECTNAME).elf $(BIN_DIR)/$(PROJECTNAME).hex $(BIN_DIR)/$(PROJECTNAME).bin
	$(STLINK_TOOL) -c port=SWD -w $(BIN_DIR)/$(PROJECTNAME).bin 0x8000000 -c port=SWD reset=SWrst

# Erase FLASH
erase_flash:
	@echo "Erasing FLASH..."
	$(STLINK_TOOL) -c $(STLINK_DEFS) -e all

# Monitor SWD debug prints
monitor:
	$(STLINK_TOOL) -c port=swd -startswv freq=84 portnumber=0 $(BUILD_DIR)/$(PROJECTNAME).log



###############################################################################
# HELP
###############################################################################
help:
	@echo "Help information"
	@echo "Usage:"
	@echo "    Clean:"
	@echo "        make clean"
	@echo ""
	@echo "    Compile:"
	@echo "        make all"
	@echo ""
	@echo "    Flash MCU:"
	@echo "        make flash"
	@echo ""
	@echo "    View SWD logs:"
	@echo "         make monitor"
	@echo ""
	@echo "    All actions:"
	@echo "         make clean && make all -j4 && make flash && make monitor"
	@echo ""
