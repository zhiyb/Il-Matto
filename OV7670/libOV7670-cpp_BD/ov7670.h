#ifndef OV7670_H
#define OV7670_H

#include <avr/io.h>
#include "connection.h"

class OV7670hw
{
public:
	//OV7670(void);
	static void init(void);
	static uint8_t read(const uint8_t addr);
	static void write(const uint8_t addr, const uint8_t data);
};

#endif
