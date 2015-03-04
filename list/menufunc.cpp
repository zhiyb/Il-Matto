#include <avr/io.h>
#include "menu.h"

using namespace menu;

bool toggle::func(void)
{
	PINB |= _BV(7);
	return false;
}
