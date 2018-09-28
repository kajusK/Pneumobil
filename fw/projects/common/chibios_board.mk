#
# Generation of chibios gpio config from stm32cubeMX project file
#

BOARD_GPIO_FILE = $(CONFDIR)/board/board_gpio.h
MX_PROJECT = $(PROJECT).ioc
MX_PATH = /opt/stm32cubemx
MX2BOARD = python $(CHIBIOS_CONTRIB)/tools/mx2board.py

$(BOARD_GPIO_FILE): $(MX_PROJECT)
	$(MX2BOARD) -p $(MX_PROJECT) -m $(MX_PATH) -o $(BOARD_GPIO_FILE)

PRE_MAKE_ALL_RULE_HOOK: $(BOARD_GPIO_FILE)
