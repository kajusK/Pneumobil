#
# FATFS files.
#
FATFSSRC = $(wildcard $(SOURCES)/drivers/fatfs/*.c) \
	   $(EXTERNAL)/fatfs/source/ff.c \
	   $(EXTERNAL)/fatfs/source/ffunicode.c

FATFSINC = $(SOURCES)/drivers/fatfs \
	   $(EXTERNAL)/fatfs/source

# Shared variables
ALLCSRC += $(FATFSSRC)
ALLINC  += $(FATFSINC)
