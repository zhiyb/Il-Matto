#ifndef LIBOV7670_CONNECTION_H
#define LIBOV7670_CONNECTION_H

// MPU6050
// PORTD
#define MPU_CTRLW	PORTD
#define MPU_CTRLR	PIND
#define MPU_CTRLD	DDRD
#define	MPU_SCL		_BV(0)
#define MPU_SDA		_BV(1)

// I2C
#define I2C_PORT	PORTD
#define I2C_PIN		PIND
#define I2C_DDR		DDRD
#define I2C_SCL		_BV(0)
#define I2C_SDA		_BV(1)

#endif
