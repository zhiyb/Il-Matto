#include <avr/io.h>
#include "menu.h"

namespace menu
{
	static tft_t *tft;
}

using namespace menu;

void menu::setTFT(tft_t *t)
{
	tft = t;
}

bool menu::toggle::func(bool enter)
{
	PINB |= _BV(7);
	return false;
}
