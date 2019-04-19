##############################################################################
# Architecture or project specific options

# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
USE_PROCESS_STACKSIZE = 0x400

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
USE_EXCEPTIONS_STACKSIZE = 0x200

# Enables the use of FPU (no, softfp, hard).
USE_FPU = no

# Mcu core definition
MCU = cortex-m0

# Use contrib repo
CHIBIOS_USE_CONTRIB = FALSE

# ChibiOS Port to use
PORT = STM32/STM32F0xx
PORTV = v6m

# Compiler optimizations
OPT = -Os

##############################################################################
# Project, sources and paths

# Define project name here
PROJECT = sdu

include ../common/paths.mk

# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f0xx.mk
# Hal mcu specific

# Define linker script file here
LDSCRIPT= $(STARTUPLD)/STM32F042x6.ld

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
USRSRC = $(wildcard $(APP_DIR)/*.c) \
	$(SOURCES)/modules/comm/session.c \
	$(SOURCES)/modules/comm/presentation.c \
	$(SOURCES)/modules/comm/application.c \
	$(SOURCES)/modules/comm/can_transport.c \
	$(SOURCES)/modules/log.c \
	$(SOURCES)/drivers/can.c \
	$(SOURCES)/drivers/wdg.c \
	$(SOURCES)/drivers/adc.c \
	$(wildcard $(SOURCES)/utils/*.c)

USRINC = $(CONFDIR) $(SOURCES) $(APP_DIR)

include ../common/chibios.mk
include ../common/chibios_board.mk

##############################################################################
# misc

include ../common/check_tools.mk
include ../common/flash.mk
