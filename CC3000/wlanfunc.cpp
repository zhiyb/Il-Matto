#include <wlan.h>
#include <hci.h>
#include <string.h>
#include "menu.h"
#include "event.h"

#define	HCI_EVENT_MASK	(HCI_EVNT_WLAN_KEEPALIVE | HCI_EVNT_WLAN_UNSOL_INIT /*|	HCI_EVNT_WLAN_ASYNC_PING_REPORT*/)

struct _wlan_full_scan_results_args_t
{
	uint32_t count;		// Number of networks found
	uint32_t status;	// The status of the scan: 0 - aged results,
				// 1 - results valid, 2	- no results
	// Result entry	(42 bytes)
	uint8_t	valid:1;	// Is result valid or not
	int8_t rssi:7;		// RSSI	value;
	uint8_t	secMode:2;	// Security mode of the	AP:
				// 0 - Open, 1 - WEP, 2	WPA, 3 WPA2
	int8_t length:6;	// SSID	name length
	int16_t	time;		// The time at which the entry has
				// entered into scans result table
	char name[32];		// SSID	name
	uint8_t	bssid[6];	// BSSID
};

bool menu::wlan::reset::func(bool enter)
{
	stop::func(enter);
	puts_P(PSTR("Starting WLAN..."));
	wlan_start(0);
	puts_P(PSTR("WLAN started."));
	// Mask	out all	non-required events from CC3000
	wlan_set_event_mask(HCI_EVENT_MASK);
	puts_P(PSTR("Event mask set."));
	return true;
}

bool menu::wlan::stop::func(bool enter)
{
	tftClean();
	puts_P(PSTR("Stopping WLAN..."));
	wlan_stop();
	puts_P(PSTR("WLAN stopped."));
	return true;
}

bool menu::wlan::scan::func(bool enter)
{
	tftClean();
#if 1
	uint32_t intv[16] = {2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000};
	int32_t	res = wlan_ioctl_set_scan_params(1, 20,	30, 2, 0x1FFF, -80, 0, 205, intv);
	printf_P(PSTR("Set params: %ld\n"), res);
#endif

	uint8_t	count =	0;
	_wlan_full_scan_results_args_t scanRes;
	do {
		res = wlan_ioctl_get_scan_results(1000,	(uint8_t *)&scanRes);
		if (count == 0)
			count =	scanRes.count;
		printf_P(PSTR("%ld (No. %lu: %lu, %u)\n"), res, count - scanRes.count, scanRes.status, scanRes.valid);
		if (scanRes.status != 1	|| !scanRes.valid)
			continue;
		printf_P(PSTR("Sec: %u, "), scanRes.secMode);
		for (uint8_t i = 0; i <	scanRes.length;	i++)
			putchar(scanRes.name[i]);
		putchar('\n');
	} while	(scanRes.count - 1);

	touch.waitForPress();
	return true;
}

bool menu::wlan::connect::func(bool enter)
{
	tftClean();
#if 0
	char ssid[] = "Network";
	res = wlan_connect(WLAN_SEC_UNSEC, ssid, strlen(ssid), 0, 0, 0);
#endif
#if 0
	char ssid[] = "ZZFNB00000017_Network";
	char key[] = "f3ei-zeb6-m35o";
#else
	char ssid[] = "Network!";
	char key[] = "WirelessNetwork";
#endif
	puts_P(PSTR("Connecting to:"));
	puts(ssid);
	puts_P(PSTR("Password:"));
	puts(key);
	int32_t	res = wlan_connect(WLAN_SEC_WPA2, ssid, strlen(ssid), 0, (uint8_t *)key, strlen(key));
#if 0
	uint8_t	bssid[6] = {0x00, 0x1F,	0xA3, 0x1F, 0xC0, 0xF7};
	res = wlan_connect(WLAN_SEC_UNSEC, 0, 0, bssid,	0, 0);
#endif
	printf_P(PSTR("WLAN connect: %ld\n"), res);

	while (touch.pressed());
	while (!touch.pressed()) {
		if (event::pool() == event::WlanConnection) {
			if (event::wlanConnected())
				puts_P(PSTR("Wlan Connected."));
			else
				puts_P(PSTR("Wlan Disconnected."));
		}
	}
	while (touch.pressed());
	return true;
}

bool menu::wlan::disconnect::func(bool enter)
{
	tftClean();
	puts_P(PSTR("Disconnecting..."));

	int32_t res = wlan_disconnect();

	if (res != 0) {
		printf_P(PSTR("Failed: %ld"), res);
		touch.waitForPress();
	}

	touch.waitForPress();
	return true;
}
