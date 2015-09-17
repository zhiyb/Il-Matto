#ifndef MENU_H
#define MENU_H

#include <list.h>
#include <tft.h>

namespace menu
{

namespace root
{
	extern listItem item;
}

namespace toggle
{
	bool func(bool enter);
}

}

#endif
