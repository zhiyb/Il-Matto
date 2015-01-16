#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <inttypes.h>

namespace i2c
{
	void init(void);
	void start(void);
	void stop(void);
	bool receiveACK(void);
	void sendACK(bool ack);
	bool write(uint8_t data);
	uint8_t read(bool ack);
}

#endif
