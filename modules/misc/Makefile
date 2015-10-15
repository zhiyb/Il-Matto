# EEPROM initialisation recognition pattern
EEMEM_INIT_PATTERN	?= 0xAA
CONFS	+= EEMEM_INIT_PATTERN

# I2C connection
# FIXME: Better not here, if multiple I2C interface is needed
I2C_PORT	?= D
I2C_PORT_SCL	?= 0
I2C_PORT_SDA	?= 1
CONFS	+= I2C_PORT I2C_PORT_SCL I2C_PORT_SDA

# File rules
NSRC	:= i2c_pin.cpp
SRC	+= $(NSRC:%=$(DIR)/%)
INCDIRS	+= $(DIR)
