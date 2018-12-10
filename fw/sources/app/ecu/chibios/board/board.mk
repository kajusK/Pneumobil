# List of all the board related files.
BOARDSRC = $(APP_DIR)/chibios/board/board.c

# Required include directories
BOARDINC = $(APP_DIR)/chibios/board

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)
