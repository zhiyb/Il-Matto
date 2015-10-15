# EEPROM initialisation recognition pattern
EEMEM_INIT_PATTERN	?= 0xAA
CONFS	+= EEMEM_INIT_PATTERN

# File rules
NSRC	:= eemem.c
SRC	+= $(NSRC:%=$(DIR)/%)
INCDIRS	+= $(DIR)
