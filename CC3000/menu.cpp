#include <macros.h>
#include <host_driver_version.h>
#include "menu.h"

namespace menu
{
void tftClean(void)
{
	tft.vsNormal();
	tft.setForeground(0x667F);
	tft.setBackground(0x0000);
	tft.setZoom(2);
	tft.clean();
}
namespace wlan
{
	namespace reset
	{
		static const char PROGMEM name[] = "Reset WLAN";
		static listItem item = {name, 0, 0, func};
	}
	namespace stop
	{
		static const char PROGMEM name[] = "Stop WLAN";
		static listItem item = {name, 0, 0, func};
	}
	namespace scan
	{
		static const char PROGMEM name[] = "Scan network";
		static listItem item = {name, 0, 0, func};
	}
	namespace connect
	{
		static const char PROGMEM name[] = "Connect network";
		static listItem item = {name, 0, 0, func};
	}
	namespace disconnect
	{
		static const char PROGMEM name[] = "Disconnect network";
		static listItem item = {name, 0, 0, func};
	}
	static const char PROGMEM name[] = "WLAN configure";
	static const listItem *items[] = {
		&reset::item, &stop::item,
		&scan::item,
		&connect::item, &disconnect::item,
	0};
	static listItem item = {name, 0, items, 0};
}
namespace nvmem
{
	namespace readSP
	{
		static const char PROGMEM name[] = "Read SP version";
		static listItem item = {name, 0, 0, func};
	}
	namespace getMAC
	{
		static const char PROGMEM name[] = "Get MAC address";
		static listItem item = {name, 0, 0, func};
	}
	namespace setMAC
	{
		static const char PROGMEM name[] = "Set MAC address";
		static listItem item = {name, 0, 0, func};
	}
	static const char PROGMEM name[] = "NVMEM operations";
	static const listItem *items[] = {
		&readSP::item, &getMAC::item, &setMAC::item,
	0};
	static listItem item = {name, 0, items, 0};
}
namespace socket
{
	namespace socket
	{
		static const char PROGMEM name[] = "socket()";
		static listItem item = {name, 0, 0, func};
	}
	namespace bind
	{
		static const char PROGMEM name[] = "bind()";
		static listItem item = {name, 0, 0, func};
	}
	namespace connect
	{
		static const char PROGMEM name[] = "connect()";
		static listItem item = {name, 0, 0, func};
	}
	namespace send
	{
		static const char PROGMEM name[] = "send()";
		static listItem item = {name, 0, 0, func};
	}
	namespace closesocket
	{
		static const char PROGMEM name[] = "closesocket()";
		static listItem item = {name, 0, 0, func};
	}
	namespace newsocket
	{
		static const char PROGMEM name[] = "newsocket()";
		static listItem item = {name, 0, 0, func};
	}
	static const char PROGMEM name[] = "Socket operations";
	static const listItem *items[] = {
		&socket::item, &bind::item, &connect::item, &send::item,
		&closesocket::item, &newsocket::item,
	0};
	static listItem item = {name, 0, items, 0};
}
namespace root
{
	static const char PROGMEM name[] = "CC3000, HDV: " TOSTRING(DRIVER_VERSION_NUMBER);
	static const listItem *items[] = {
		&wlan::item, &nvmem::item, &socket::item,
	0};
	listItem item = {name, 0, items, 0};
}
}
