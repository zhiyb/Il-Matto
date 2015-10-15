# RGB LED connection
RGBLED_PORT	?= D
RGBLED_DIN	?= 7
# Number of RGB LEDs in the line
RGBLED_NUM	?= 8
CONFS	+= RGBLED_PORT RGBLED_DIN RGBLED_NUM

# File rules
NSRC	:= rgbled.c
SRC	+= $(NSRC:%=$(DIR)/%)
INCDIRS	+= $(DIR)
