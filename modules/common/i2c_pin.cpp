#include <util/delay.h>
#include "i2c.h"
#include "macros.h"

#ifndef I2C_PORT
#define I2C_PORT	D
#endif
#ifndef I2C_PORT_SCL
#define I2C_PORT_SCL	0
#endif
#ifndef I2C_PORT_SDA
#define I2C_PORT_SDA	1
#endif

// I2C
#define I2C_W	CONCAT_E(PORT, I2C_PORT)
#define I2C_R	CONCAT_E(PIN, I2C_PORT)
#define I2C_D	CONCAT_E(DDR, I2C_PORT)
#define I2C_SCL	_BV(I2C_PORT_SCL)
#define I2C_SDA	_BV(I2C_PORT_SDA)


#define DELAY_tBUF()	_delay_us(2)
#define DELAY_tSU_STA()	_delay_us(1)
#define DELAY_tHD_STA()	DELAY_tSU_STA()
#define DELAY_tSU_STO()	_delay_us(1)
#define DELAY_tHIGH()	_delay_us(1)
#define DELAY_tLOW()	_delay_us(2)

void i2c::init(void)
{
	wait();
	stop();
}

void i2c::wait(void)
{
	I2C_D &= ~I2C_SCL;
	I2C_W |= I2C_SCL;
	DELAY_tHIGH();
	while (!(I2C_R & I2C_SCL));
	I2C_D |= I2C_SCL;
}

void i2c::start(void)
{
	wait();
	I2C_D |= I2C_SDA;
	I2C_W |= I2C_SDA;
	DELAY_tSU_STA();
	I2C_W &= ~I2C_SDA;
	DELAY_tHD_STA();
	I2C_W &= ~I2C_SCL;
}

void i2c::stop(void)
{
	I2C_D |= I2C_SDA;
	I2C_W &= ~I2C_SDA;
	I2C_W |= I2C_SCL;
	DELAY_tSU_STO();
	I2C_W |= I2C_SDA;
	DELAY_tBUF();
}

bool i2c::write(uint8_t data)
{
	I2C_D |= I2C_SDA;
	for (uint8_t i = 0; i < 8; i++) {
		if (data & 0x80)
			I2C_W |= I2C_SDA;
		else
			I2C_W &= ~(I2C_SDA);
		data <<= 1;
		DELAY_tLOW();
		I2C_W |= I2C_SCL;
		DELAY_tHIGH();
		I2C_W &= ~I2C_SCL;
	}

	// Receive ACK
	I2C_D &= ~I2C_SDA;
	I2C_W |= I2C_SDA;
	DELAY_tLOW();
	I2C_W |= I2C_SCL;
	DELAY_tHIGH();
	bool ack = !(I2C_R & I2C_SDA);
	I2C_W &= ~I2C_SCL;
	return ack;
}

uint8_t i2c::read(bool ack)
{
	uint8_t data = 0;
	I2C_D &= ~I2C_SDA;
	I2C_W |= I2C_SDA;
	for (uint8_t i = 0; i < 8; i++) {
		DELAY_tLOW();
		I2C_W |= I2C_SCL;
		DELAY_tHIGH();
		data <<= 1;
		if (I2C_R & I2C_SDA)
			data |= 1;
		I2C_W &= ~I2C_SCL;
	}

	// Send ACK
	if (ack)
		I2C_W &= ~I2C_SDA;
	else
		I2C_W |= I2C_SDA;
	DELAY_tLOW();
	I2C_D |= I2C_SDA;
	I2C_W |= I2C_SCL;
	DELAY_tHIGH();
	I2C_W &= ~I2C_SCL;
	return data;
}
