#include <avr/io.h>
#include "menu.h"

using namespace menu;

bool menu::toggle::func(bool enter)
{
	PINB |= _BV(7);
	return false;
}
