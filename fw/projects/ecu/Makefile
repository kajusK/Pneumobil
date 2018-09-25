##############################################################################
# Compiler options here.
USE_OPT = -Os -ggdb -fomit-frame-pointer -falign-functions=16 -std=gnu11 \
		--specs=nano.specs --specs=nosys.specs

# C specific options here (added to USE_OPT).
USE_COPT =

# C++ specific options here (added to USE_OPT).
USE_CPPOPT = -fno-rtti

# Enable this if you want the linker to remove unused code and data
USE_LINK_GC = yes

# Linker extra options here.
USE_LDOPT =

# Enable this if you want link time optimizations (LTO)
USE_LTO = yes

# If enabled, this option allows to compile the application in THUMB mode.
USE_THUMB = yes

# Enable this if you want to see the full log while compiling.
USE_VERBOSE_COMPILE = yes

# If enabled, this option makes the build process faster by not compiling
# modules not used in the current configuration.
USE_SMART_BUILD = yes

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

##############################################################################
# Project, sources and paths

# Define project name here
PROJECT = ecu

SOURCES = ../../sources
EXTERNAL = $(SOURCES)/external
APP_DIR = $(SOURCES)/app/$(PROJECT)
CHIBIOS = $(EXTERNAL)/ChibiOS
CONFDIR = $(APP_DIR)/chibios

# Licensing files.
include $(CHIBIOS)/os/license/license.mk
# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f0xx.mk
# HAL-OSAL files (optional).
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32F0xx/platform.mk
include $(APP_DIR)/chibios/board/board.mk
include $(CHIBIOS)/os/hal/osal/rt/osal.mk
# RTOS files (optional).
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC/mk/port_v6m.mk
# Other files (optional).
include $(CHIBIOS)/os/hal/lib/streams/streams.mk

# Define linker script file here
LDSCRIPT= $(STARTUPLD)/STM32F072xB.ld

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
USRSRC = $(wildcard $(APP_DIR)/*.c) \
	$(wildcard $(SOURCES)/modules/*.c) \
	$(wildcard $(SOURCES)/modules/*/*.c) \
	$(wildcard $(SOURCES)/drivers/*.c) \
	$(wildcard $(SOURCES)/utils/*.c)

CSRC = $(ALLCSRC) \
	$(USRSRC)

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC = $(ALLCPPSRC)

# C sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
ACSRC =

# C++ sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
ACPPSRC =

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
TCSRC =

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
TCPPSRC =

# List ASM source files here
ASMSRC = $(ALLASMSRC)
ASMXSRC = $(ALLXASMSRC)

INCDIR = $(ALLINC) $(CONFDIR) $(SOURCES)

##############################################################################
# Compiler settings
MCU  = cortex-m0

#TRGT = arm-elf-
TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CPPC = $(TRGT)g++
# Enable loading with g++ only if you need C++ runtime support.
# NOTE: You can use C++ even without C++ support if you are careful. C++
#       runtime support makes code size explode.
LD   = $(TRGT)gcc
#LD   = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
AR   = $(TRGT)ar
OD   = $(TRGT)objdump
SZ   = $(TRGT)size
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary

# ARM-specific options here
AOPT =

# THUMB-specific options here
TOPT = -mthumb -DTHUMB

# Define C warning options here
CWARN = -Wall -Wextra -Wstrict-prototypes -pedantic -Wundef -Wshadow \
	-Wredundant-decls -Wmissing-prototypes

# Define C++ warning options here
CPPWARN = -Wall -Wextra -Wundef

##############################################################################
# Start of user section
#

# List all user C define here, like -D_DEBUG=1
UDEFS =

# Define ASM defines here
UADEFS =

# List all user directories here
UINCDIR =

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

STFLASH	= $(shell which st-flash)

STYLECHECK	:= /checkpatch.pl
STYLECHECK_FLAGS	:= --no-tree -f --terse --mailback
STYLECHECK_FILES	:= $(USRSRC) \
    $(shell find $(SOURCES) -name '*.h' | grep -v $(EXTERNAL) | grep -v $(SOURCES)/tests)

CODECHECK	:= cppcheck
CODECHECK_FLAGS := --enable=warning,performance -j 4 \
    $(addprefix -I, $(SOURCES)) \
    --template=gcc --suppress=missingIncludeSystem \
    --suppress=unusedFunction --suppress=unmatchedSuppression \
    --suppress=missingInclude --platform=unix32 -q

##############################################################################

RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk
include $(RULESPATH)/rules.mk

.PHONY : stylecheck codecheck flash
flash: build/$(PROJECT).bin
	$(STFLASH) write $< 0x8000000

codecheck:
	@$(CODECHECK) $(CODECHECK_FLAGS) $(STYLECHECK_FILES)

stylecheck:
	$(OPENCM3_SCRIPT_DIR)$(STYLECHECK) $(STYLECHECK_FLAGS) $(STYLECHECK_FILES)
