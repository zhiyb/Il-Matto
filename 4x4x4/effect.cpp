#include <avr/io.h>
#include <util/delay.h>
#include "led.h"
#include "effect.h"

using namespace led;

void effect::breath(bool in)
{
	enablePWM(true);
	if (in)
		for (OCR1B = 0; OCR1B != Max; OCR1B++)
			_delay_us(Breath);
	else {
		for (OCR1B = Max; OCR1B != 0; OCR1B--)
			_delay_us(Breath);
		delay();
	}
}

void effect::scan(void)
{
	enablePWM(false);
	for (uint8_t i = 0; i < 4; i++) {
		for (uint8_t j = 0; j < 4; j++)
			for (uint8_t k = 0; k < 4; k++) {
				if (i % 2)
					buff[i] = 0x8000 >> (j % 2 ? \
						  j * 4 + k : \
						  j * 4 + 3 - k);
				else
					buff[i] = 1 << (j % 2 ? \
						  j * 4 + 3 - k : \
						  j * 4 + k);
				_delay_us(Scan);
			}
		buff[i] = 0;
	}
	fill(false);
}

void effect::expand(bool br)
{
	enablePWM(br);
	setPWM(0);
	/*fill(false);
	if (br) {
		breath(In);
		breath(Out);
	} else
		_delay_us(Expand);*/
	buff[1] = 0x0660;
	buff[2] = 0x0660;
	if (br) {
		breath(In);
		breath(Out);
	} else
		_delay_us(Expand);
	buff[0] = 0x0660;
	buff[1] = 0x6FF6;
	buff[2] = 0x6FF6;
	buff[3] = 0x0660;
	if (br) {
		breath(In);
		breath(Out);
	} else
		_delay_us(Expand);
	buff[0] = 0x6FF6;
	buff[1] = 0xFFFF;
	buff[2] = 0xFFFF;
	buff[3] = 0x6FF6;
	if (br) {
		breath(In);
		breath(Out);
	} else
		_delay_us(Expand);
	fill(true);
	if (br) {
		breath(In);
		breath(Out);
	} else
		_delay_us(Expand);
}

void effect::shrink(bool br)
{
	enablePWM(br);
	setPWM(0);
	fill(true);
	if (br) {
		breath(In);
		breath(Out);

	} else
		_delay_us(Expand);
	buff[0] = 0x6FF6;
	buff[1] = 0xFFFF;
	buff[2] = 0xFFFF;
	buff[3] = 0x6FF6;
	if (br) {
		breath(In);
		breath(Out);
	} else
		_delay_us(Expand);
	buff[0] = 0x0660;
	buff[1] = 0x6FF6;
	buff[2] = 0x6FF6;
	buff[3] = 0x0660;
	if (br) {
		breath(In);
		breath(Out);
	} else
		_delay_us(Expand);
	buff[1] = 0x0660;
	buff[2] = 0x0660;
	if (br) {
		breath(In);
		breath(Out);
	} else
		_delay_us(Expand);
	/*fill(false);
	if (br) {
		breath(In);
		breath(Out);
	} else
		_delay_us(Expand);*/
}

void effect::circle(bool in, bool br)
{
	enablePWM(br);
	setPWM(0);
	if (in) {
		/*fill(false);
		if (br) {
			breath(In);
			breath(Out);
		} else
			_delay_us(Expand);*/
		buff[0] = 0x9009;
		buff[1] = 0x0000;
		buff[2] = 0x0000;
		buff[3] = 0x9009;
		if (br) {
			breath(In);
			breath(Out);
		} else
			_delay_us(Expand);
		buff[0] = 0x6996;
		buff[1] = 0x9009;
		buff[2] = 0x9009;
		buff[3] = 0x6996;
		if (br) {
			breath(In);
			breath(Out);
		} else
			_delay_us(Expand);
		buff[0] = 0x0660;
		buff[1] = 0x6996;
		buff[2] = 0x6996;
		buff[3] = 0x0660;
		if (br) {
			breath(In);
			breath(Out);
		} else
			_delay_us(Expand);
		buff[0] = 0x0000;
		buff[1] = 0x0660;
		buff[2] = 0x0660;
		buff[3] = 0x0000;
		if (br) {
			breath(In);
			breath(Out);
		} else
			_delay_us(Expand);
		/*fill(false);
		if (br) {
			breath(In);
			breath(Out);
		} else
			_delay_us(Expand);*/
	} else {
		/*fill(false);
		if (br) {
			breath(In);
			breath(Out);
		} else
			_delay_us(Expand);*/
		buff[0] = 0x0000;
		buff[1] = 0x0660;
		buff[2] = 0x0660;
		buff[3] = 0x0000;
		if (br) {
			breath(In);
			breath(Out);
		} else
			_delay_us(Expand);
		buff[0] = 0x0660;
		buff[1] = 0x6996;
		buff[2] = 0x6996;
		buff[3] = 0x0660;
		if (br) {
			breath(In);
			breath(Out);
		} else
			_delay_us(Expand);
		buff[0] = 0x6996;
		buff[1] = 0x9009;
		buff[2] = 0x9009;
		buff[3] = 0x6996;
		if (br) {
			breath(In);
			breath(Out);
		} else
			_delay_us(Expand);
		buff[0] = 0x9009;
		buff[1] = 0x0000;
		buff[2] = 0x0000;
		buff[3] = 0x9009;
		if (br) {
			breath(In);
			breath(Out);
		} else
			_delay_us(Expand);
		/*fill(false);
		if (br) {
			breath(In);
			breath(Out);
		} else
			_delay_us(Expand);*/
	}
}

void effect::delay(void)
{
	_delay_us(General);
}
