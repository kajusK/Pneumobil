##############################################################################
# Architecture or project specific options

# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
USE_PROCESS_STACKSIZE = 0x800

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
USE_EXCEPTIONS_STACKSIZE = 0x400

# Enables the use of FPU (no, softfp, hard).
USE_FPU = hard

# Mcu core definition
MCU = cortex-m4

# Use contrib repo
CHIBIOS_USE_CONTRIB = FALSE

# ChibiOS Port to use
PORT = STM32/STM32F4xx
PORTV = v7m

# Compiler optimizations
OPT = -O0

##############################################################################
# Project, sources and paths

# Define project name here
PROJECT = hmi

include ../common/paths.mk

# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f4xx.mk
# Hal mcu specific

# Define linker script file here
LDSCRIPT= $(STARTUPLD)/STM32F412xE.ld

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
USRSRC = $(wildcard $(APP_DIR)/*.c) \
	$(wildcard $(APP_DIR)/*/*.c) \
	$(wildcard $(SOURCES)/modules/*.c) \
	$(wildcard $(SOURCES)/modules/*/*.c) \
	$(SOURCES)/drivers/can.c \
	$(SOURCES)/drivers/eeprom.c \
	$(SOURCES)/drivers/i2c.c \
	$(SOURCES)/drivers/uart.c \
	$(SOURCES)/drivers/wdg.c \
	$(SOURCES)/drivers/rtc.c \
	$(SOURCES)/drivers/spi.c \
	$(SOURCES)/drivers/rfm69.c \
	$(wildcard $(SOURCES)/drivers/ugfx/*.c) \
	$(wildcard $(SOURCES)/utils/*.c)

USRINC = $(CONFDIR) $(SOURCES) $(APP_DIR)

include ../common/fatfs.mk
include ../common/ugfx.mk
include ../common/chibios.mk
include ../common/chibios_board.mk

##############################################################################
# misc

include ../common/check_tools.mk
include ../common/flash.mk
