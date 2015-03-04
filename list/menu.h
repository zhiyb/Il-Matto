#ifndef MENU_H
#define MENU_H

#include <list.h>
#include <tft.h>

namespace menu
{

void setTFT(tft_t *t);

namespace root
{
	extern listItem item;
}

namespace toggle
{
	bool func(void);
}

}

#endif
