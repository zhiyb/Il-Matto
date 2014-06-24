#ifndef _LED_H
#define _LED_H

#include <inttypes.h>

namespace led
{
	enum States {On = true, Off = false};

	void init(void);
	void fill(bool state = Off);

	// Row, Column, Colour
	extern volatile uint16_t buff[4];
};

#endif
