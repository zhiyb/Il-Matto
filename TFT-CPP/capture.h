#ifndef CAPTURE_H
#define CAPTURE_H

#include <tft.h>

// UART baudrate (750kbps)
#define CAPTURE_BAUD	(F_CPU / 8 / (1 + 1))

namespace capture
{
	void init(tft_t *t);
	void enable(void);
	void disable(void);
}

#endif
