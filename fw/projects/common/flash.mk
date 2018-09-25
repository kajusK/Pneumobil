#
# Firmware flashing
#

STFLASH	= $(shell which st-flash)

flash: build/$(PROJECT).bin
	$(STFLASH) write $< 0x8000000
