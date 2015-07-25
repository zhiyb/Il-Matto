#include <nvmem.h>
#include "menu.h"

bool menu::nvmem::readSP::func(bool enter)
{
	tftClean();
	puts_P(PSTR("Reading NVMEM..."));

	uint8_t	ver[2];
	int32_t res = nvmem_read_sp_version(ver);

	if (res != 0)
		printf_P(PSTR("Failed: %ld"), res);
	else
		printf_P(PSTR("SP version: %u.%u\n"), ver[0], ver[1]);

	touch.waitForPress();
	return true;
}

bool menu::nvmem::getMAC::func(bool enter)
{
	tftClean();
	puts_P(PSTR("Reading NVMEM..."));

	uint8_t	mac[6];
	int32_t res = nvmem_get_mac_address(mac);
	if (res != 0)
		printf_P(PSTR("Failed: %ld"), res);
	else {
		printf_P(PSTR("MAC address:\n"));
		for (uint8_t i = 0; i != 6; i++) {
			printf_P(PSTR("%02X"), mac[i]);
			putchar(i != 5 ? ':' : '\n');
		}
	}

	touch.waitForPress();
	return true;
}

bool menu::nvmem::setMAC::func(bool enter)
{
	tftClean();
	puts_P(PSTR("Writing NVMEM..."));

	uint8_t	mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
	int32_t res = nvmem_set_mac_address(mac);
	if (res != 0)
		printf_P(PSTR("Failed: %ld"), res);
	else {
		printf_P(PSTR("MAC address set to:\n"));
		for (uint8_t i = 0; i != 6; i++) {
			printf_P(PSTR("%02X"), mac[i]);
			putchar(i != 5 ? ':' : '\n');
		}
		puts_P(PSTR("Need to reset WLAN"));
	}

	touch.waitForPress();
	if (res == 0)
		wlan::reset::func(enter);
	return true;
}
