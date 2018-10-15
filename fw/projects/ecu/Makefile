##############################################################################
# Architecture or project specific options

# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
USE_PROCESS_STACKSIZE = 0x200

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
USE_EXCEPTIONS_STACKSIZE = 0x200

# Enables the use of FPU (no, softfp, hard).
USE_FPU = no

# Mcu core definition
MCU = cortex-m0

##############################################################################
# Project, sources and paths

# Define project name here
PROJECT = ecu

include ../common/paths.mk

# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f0xx.mk
# Hal mcu specific
include $(CHIBIOS)/os/hal/ports/STM32/STM32F0xx/platform.mk
# Define linker script file here
LDSCRIPT= $(STARTUPLD)/STM32F072xB.ld

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
USRSRC = $(wildcard $(APP_DIR)/*.c) \
	$(wildcard $(SOURCES)/modules/*.c) \
	$(wildcard $(SOURCES)/modules/*/*.c) \
	$(wildcard $(SOURCES)/drivers/*.c) \
	$(wildcard $(SOURCES)/utils/*.c)

USRINC = $(CONFDIR) $(SOURCES)

include ../common/chibios.mk
include ../common/chibios_board.mk
include ../common/chibios_contrib.mk

##############################################################################
# misc

include ../common/check_tools.mk
include ../common/flash.mk

