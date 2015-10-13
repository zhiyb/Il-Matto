TFT_HARDWARE	?= ili9341_parallel
CONFS	+= TFT_HARDWARE

# Use shifting up (slow!) instead of clean screen after screen full
CONFS_D	+= TFT_SCROLL
# Vertical scrolling functionality enable
CONFS_D	+= TFT_VERTICAL_SCROLLING
# Enable draw in range checking
CONFS_D	+= TFT_CHECKING
# (Experimental) Multiple font support, zooming not supported yet
CONFS_D += TFT_FONTS

# Module specific settings
# Ability to read pixels (for screen shift up operation)
CONFS_D	+= TFT_READ_AVAILABLE
ifeq ($(TFT_HARDWARE), ili9341_parallel)
	# TFT port connection
	TFT_PORT_CTRL	?= A
	TFT_PORT_DATA	?= C
	CONFS	+= TFT_PORT_CTRL TFT_PORT_DATA
	TFT_READ_AVAILABLE = y
else ifeq ($(TFT_HARDWARE), ili9341_serial4_wo)
	# TFT port connection
	TFT_PORT	= B
	CONFS	+= TFT_PORT
	TFT_READ_AVAILABLE = n
else ifeq ($(TFT_HARDWARE), st7735s_serial4_wo)
	# TFT port connection
	TFT_PORT	= D
	CONFS	+= TFT_PORT
	TFT_READ_AVAILABLE = n
endif

# File rules
NSRC	:= ascii.c tft.cpp
SRC	+= $(NSRC:%=$(DIR)/%)
INCDIRS	+= $(DIR) $(DIR)/$(TFT_HARDWARE)
