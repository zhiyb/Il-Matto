#ifndef OV7670_H
#define OV7670_H

#include <avr/io.h>
#include <inttypes.h>
#include "ov7670regs.h"

struct regval_list {
	unsigned char reg_num;
	unsigned char value;
};

namespace OV7670
{
	void init(void);
	uint8_t read(const uint8_t addr);
	void write(const uint8_t addr, const uint8_t data);
	void writeArray(struct regval_list *vals);
	void writeArray(uint8_t *vals);
	void reset(void);
	void capture(void);
	void startCapture(void);
	void enableFIFO(bool e);
	uint8_t readFIFO(void);
}

#endif
