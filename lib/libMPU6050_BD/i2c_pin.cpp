#include <util/delay.h>
#include "i2c.h"
#include "connection.h"

#define DELAY_tBUF()	_delay_us(2)
#define DELAY_tSU_STA()	_delay_us(1)
#define DELAY_tHD_STA()	DELAY_tSU_STA()
#define DELAY_tSU_STO()	_delay_us(1)
#define DELAY_tHIGH()	_delay_us(1)
#define DELAY_tLOW()	_delay_us(2)

void i2c::init(void)
{
	I2C_DDR &= ~I2C_SDA;
	I2C_DDR |= I2C_SCL;
	stop();
}

void i2c::start(void)
{
	I2C_DDR |= I2C_SDA;
	I2C_PORT |= I2C_SDA | I2C_SCL;
	DELAY_tSU_STA();
	I2C_PORT &= ~I2C_SDA;
	DELAY_tHD_STA();
	I2C_PORT &= ~I2C_SCL;
}

void i2c::stop(void)
{
	I2C_PORT &= ~I2C_SDA;
	I2C_PORT |= I2C_SCL;
	I2C_DDR |= I2C_SDA;
	DELAY_tSU_STO();
	I2C_PORT |= I2C_SDA;
	DELAY_tBUF();
}

bool i2c::receiveACK(void)
{
	I2C_DDR &= ~I2C_SDA;
	I2C_PORT |= I2C_SDA;
	DELAY_tLOW();
	I2C_PORT |= I2C_SCL;
	DELAY_tHIGH();
	bool ack = !(I2C_PIN & I2C_SDA);
	I2C_PORT &= ~I2C_SCL;
	return ack;
}

void i2c::sendACK(bool ack)
{
	// SCL = 0, SDA = ?;
	if (ack)
		I2C_PORT &= ~I2C_SDA;
	else
		I2C_PORT |= I2C_SDA;
	I2C_DDR |= I2C_SDA;
	DELAY_tLOW();
	I2C_PORT |= I2C_SCL;
	DELAY_tHIGH();
	I2C_PORT &= ~I2C_SCL;
	//I2C_PORT |= I2C_SDA;
}

bool i2c::write(uint8_t data)
{
	I2C_DDR |= I2C_SDA;
	for (uint8_t i = 0; i < 8; i++) {
		if (data & 0x80)
			I2C_PORT |= I2C_SDA;
		else
			I2C_PORT &= ~(I2C_SDA);
		data <<= 1;
		DELAY_tLOW();
		I2C_PORT |= I2C_SCL;
		DELAY_tHIGH();
		I2C_PORT &= ~I2C_SCL;
		//DELAY_tLOW();
	}
	return receiveACK();
}

uint8_t i2c::read(bool ack)
{
	uint8_t data = 0;
	I2C_DDR &= ~I2C_SDA;
	I2C_PORT |= I2C_SDA;
	for (uint8_t i = 0; i < 8; i++) {
		data <<= 1;
		DELAY_tLOW();
		I2C_PORT |= I2C_SCL;
		DELAY_tHIGH();
		if (I2C_PIN & I2C_SDA)
			data |= 1;
		I2C_PORT &= ~I2C_SCL;
	}
	sendACK(ack);
	return data;
}
