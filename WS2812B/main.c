// Author: Yubo Zhi (normanzyb@gmail.com)

#include <avr/io.h>
#include "rgbled.h"

int main(void)
{
	rgbLED_init();
	rgbLED_refresh();

loop:
	goto loop;
	return 1;
}
