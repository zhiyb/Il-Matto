#ifndef MENU_H
#define MENU_H

#include <list.h>
#include <tft.h>
#include <rtouch.h>

extern tft_t tft;
extern rTouch touch;

namespace menu
{
void tftClean(void);
namespace wlan
{
	namespace reset{bool func(bool enter);}
	namespace stop{bool func(bool enter);}
	namespace scan{bool func(bool enter);}
	namespace connect{bool func(bool enter);}
	namespace disconnect{bool func(bool enter);}
}
namespace nvmem
{
	namespace readSP{bool func(bool enter);}
	namespace getMAC{bool func(bool enter);}
	namespace setMAC{bool func(bool enter);}
}
namespace socket
{
	namespace socket{bool func(bool enter);}
	namespace bind{bool func(bool enter);}
	namespace connect{bool func(bool enter);}
	namespace send{bool func(bool enter);}
	namespace closesocket{bool func(bool enter);}
	namespace newsocket{bool func(bool enter);}
}
namespace root {extern listItem item;}
}

#endif
