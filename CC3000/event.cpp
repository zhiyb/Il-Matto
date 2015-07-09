#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdint.h>
#include <hci.h>
#include <netapp.h>
#include "event.h"

#define	NETAPP_IPCONFIG_MAC_OFFSET	20

namespace event
{
	static volatile Status status;
	static volatile bool s_dhcpFin, s_wlanConnected;
	static volatile Result result;
}

void event::init(void)
{
	status = None;
	s_dhcpFin = false;
	s_wlanConnected = false;
}

bool event::dhcpFinished(void)
{
	return s_dhcpFin;
}

bool event::wlanConnected(void)
{
	return s_wlanConnected;
}

const uint32_t event::ipAddress(void)
{
	return result.ip;
}

event::Status event::pool(void)
{
	Status s = status;
	status = None;
	return s;
}

void event::usynchCallback(long lEventType, char *data, unsigned char length)
{
	switch (lEventType) {
	case HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE:
		status = SmartConfigFinished;
		break;
	case HCI_EVNT_WLAN_UNSOL_CONNECT:
		s_wlanConnected = true;
		status = WlanConnection;
		break;
	case HCI_EVNT_WLAN_UNSOL_DISCONNECT:
		s_wlanConnected = false;
		s_dhcpFin = false;
		status = WlanConnection;
		break;
	case HCI_EVNT_WLAN_UNSOL_DHCP: {
		// Notes:
		// 1) IP config	parameters are received	swapped
		// 2) IP config	parameters are valid only if status is OK,
		// i.e.	ulCC3000DHCP becomes 1

		// Only	if status is OK, the flag is set to 1 and the
		// addresses are valid.
		status = DHCP;
		bool dhcpFin = *(data + NETAPP_IPCONFIG_MAC_OFFSET) == 0;
		s_dhcpFin = dhcpFin;
		if (dhcpFin) {
			//printf_P(PSTR("IP: "));
			result.ip = *data;
			/*i = 3;
			do {
				s_ip[3 - i] = data[i];
				//printf("%u", (uint8_t)data[i]);
				//putchar(i != 0 ? '.' : '\n');
			} while	(i-- !=	0);*/
		}
		break;
	}
	case HCI_EVNT_WLAN_ASYNC_PING_REPORT: {
		netapp_pingreport_args_t *report = (netapp_pingreport_args_t *)data;
		printf_P(PSTR("Packets:	%lu/%lu\n"), report->packets_received, report->packets_sent);
		printf_P(PSTR("Round time: %lu(%lu, %lu)\n"), report->avg_round_time, report->min_round_time, report->max_round_time);
		break;
	}
	case HCI_EVENT_CC3000_CAN_SHUT_DOWN:
		break;
	default:
		printf_P(PSTR("Event: %lX, %u\n"), lEventType, length);
	}
}
