#ifndef _LED_H
#define _LED_H

#include <inttypes.h>

namespace led
{
	enum States {On = true, Off = false};
	enum Num {Max = 1024, Top = 1030};

	void init(void);
	void fill(bool state = Off);

	void enable(bool e);
	bool enabled(void);
	void enablePWM(bool e);
	bool PWMEnabled(void);
	void setPWM(uint16_t pwm);

	// Row, Column, Colour
	extern volatile uint16_t buff[4];
};

#endif
