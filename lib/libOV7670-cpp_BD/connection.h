#ifndef LIBOV7670_CONNECTION_H
#define LIBOV7670_CONNECTION_H

// OV7670 (CF7670C-V2)
// PORTD
#define OV_CTRLW	PORTD
#define OV_CTRLR	PIND
#define OV_CTRLD	DDRD
#define	OV_SCL		_BV(0)
#define OV_SDA		_BV(1)
#define OV_VSYNC	_BV(2)
#define OV_HREF		_BV(3)
#define OV_WEN		_BV(4)
#define OV_RRST		_BV(5)
#define OV_OE		_BV(6)
#define OV_RCLK		_BV(7)
// PORTB
#define OV_DATAW	PORTB
#define OV_DATAR	PINB
#define OV_DATAD	DDRB

// I2C
#define I2C_PORT	OV_CTRLW
#define I2C_PIN		OV_CTRLR
#define I2C_DDR		OV_CTRLD
#define I2C_SCL		OV_SCL
#define I2C_SDA		OV_SDA

#endif
