#ifndef LIBOV7670_CONNECTION_H
#define LIBOV7670_CONNECTION_H

#include <macros.h>

#ifndef OV7670_PORT_CTRL
#define OV7670_PORT_CTRL	D
#endif
#ifndef OV7670_PORT_DATA
#define OV7670_PORT_DATA	B
#endif

// OV7670 (CF7670C-V2)
// Control port
#define OV_CTRLW	CONCAT_E(PORT, OV7670_PORT_CTRL)
#define OV_CTRLR	CONCAT_E(PIN, OV7670_PORT_CTRL)
#define OV_CTRLD	CONCAT_E(DDR, OV7670_PORT_CTRL)
#define	OV_SCL		_BV(0)
#define OV_SDA		_BV(1)
#define OV_VSYNC	_BV(2)
#define OV_HREF		_BV(3)
#define OV_WEN		_BV(4)
#define OV_RRST		_BV(5)
#define OV_OE		_BV(6)
#define OV_RCLK		_BV(7)
// Data port
#define OV_DATAW	CONCAT_E(PORT, OV7670_PORT_DATA)
#define OV_DATAR	CONCAT_E(PIN, OV7670_PORT_DATA)
#define OV_DATAD	CONCAT_E(DDR, OV7670_PORT_DATA)

// I2C
#define I2C_PORT	OV_CTRLW
#define I2C_PIN		OV_CTRLR
#define I2C_DDR		OV_CTRLD
#define I2C_SCL		OV_SCL
#define I2C_SDA		OV_SDA

#endif
