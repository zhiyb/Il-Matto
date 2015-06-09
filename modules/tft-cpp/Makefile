# TFT port connection
TFT_PORT_CTRL	?= A
TFT_PORT_DATA	?= C
CONFS	+= TFT_PORT_CTRL TFT_PORT_DATA

# Vertical scrolling functionality enable
CONFS_D	+= TFT_VERTICALSCROLLING

# File rules
NSRC	:= ascii.c tft.cpp
SRC	+= $(NSRC:%=$(DIR)/%)
INCDIRS	+= $(DIR)
