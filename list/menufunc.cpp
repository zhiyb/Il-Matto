#include <avr/io.h>
#include "menu.h"

bool func_toggle(void)
{
	PINB |= _BV(7);
	return false;
}
