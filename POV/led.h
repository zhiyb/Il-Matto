#ifndef _LED_H
#define _LED_H

#include <inttypes.h>

namespace led
{
	void init(void);
	void send(uint8_t data);
}

#endif
