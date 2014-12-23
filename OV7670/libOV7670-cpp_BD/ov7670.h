#ifndef OV7670_H
#define OV7670_H

#include <avr/io.h>
#include "connection.h"

namespace OV7670
{
	void init(void);
	uint8_t read(const uint8_t addr);
	void write(const uint8_t addr, const uint8_t data);
	void reset(void);
}

#endif
