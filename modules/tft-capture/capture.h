#ifndef CAPTURE_H
#define CAPTURE_H

#ifdef TFT_READ_AVAILABLE

#include <tft.h>

// UART baudrate (750kbps)
#define CAPTURE_BAUD	(F_CPU / 8 / (1 + 1))

namespace capture
{
	void init();
	void enable();
	void disable();
}

#endif	// TFT_READ_AVAILABLE

#endif
