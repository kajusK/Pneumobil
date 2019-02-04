#
# uGFX graphical library
#

GFXLIB = $(EXTERNAL)/uGFX

include $(GFXLIB)/gfx.mk
include $(GFXLIB)/drivers/gdisp/SSD1963/driver.mk
include $(GFXLIB)/drivers/ginput/touch/ADS7843/driver.mk

ALLCSRC += $(GFXSRC)
ALLINC  += $(GFXINC) $(SOURCES)/drivers/ugfx
