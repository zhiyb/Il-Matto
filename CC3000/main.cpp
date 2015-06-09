#include <avr/io.h>
#include <avr/interrupt.h>
#include <tft.h>
#include <spi.h>
#include <host_driver_version.h>
#include <wlan.h>
#include <hci.h>
#include <nvmem.h>
#include <netapp.h>

#define	NETAPP_IPCONFIG_MAC_OFFSET	(20)
#define	HCI_EVENT_MASK	(HCI_EVNT_WLAN_KEEPALIVE | HCI_EVNT_WLAN_UNSOL_INIT /*|	HCI_EVNT_WLAN_ASYNC_PING_REPORT*/)

struct _wlan_full_scan_results_args_t
{
	uint32_t count;			// Number of networks found
	uint32_t status;		// The status of the scan: 0 - aged results,
					// 1 - results valid, 2	- no results
					// Result entry	(42 bytes)
	uint8_t	valid:1;			// Is result valid or not
	int8_t rssi:7;				// RSSI	value;
	uint8_t	secMode:2;			// Security mode of the	AP:
						// 0 - Open, 1 - WEP, 2	WPA, 3 WPA2
	int8_t length:6;			// SSID	name length
	int16_t	time;				// The time at which the entry has entered into	scans result table
	char name[32];				// SSID	name
	uint8_t	bssid[6];			// BSSID
};

tft_t tft;

volatile unsigned long ulSmartConfigFinished, ulCC3000Connected,ulCC3000DHCP,
			OkToDoShutDown,	ulCC3000DHCP_configured;
volatile unsigned char ucStopSmartConfig;

unsigned char printOnce	= 1;

// Simple Config Prefix
const char aucCC3000_prefix[] =	{'T', 'T', 'T'};

long ReadWlanInterruptPin(void)
{
	return SPI_R & CC3000_INT;
}

void WlanInterruptEnable()
{
	PCICR |= _BV(PCIE3);
	PCMSK3 |= CC3000_INT;
}

void WlanInterruptDisable()
{
	PCMSK3 &= ~CC3000_INT;
}

void WriteWlanPin(unsigned char	val)
{
	if (val)
		SPI_W |= CC3000_EN;
	else
		SPI_W &= ~CC3000_EN;
}

char *sendPatch(unsigned long *Length)
{
	*Length	= 0;
	return NULL;
}

//*****************************************************************************
//
//! CC3000_UsynchCallback
//!
//! @param  lEventType	 Event type
//! @param  data
//! @param  length
//!
//! @return none
//!
//! @brief  The	function handles asynchronous events that come from CC3000
//!	    device and operates	a LED1 to have an on-board indication
//
//*****************************************************************************
void CC3000_UsynchCallback(long	lEventType, char *data,	unsigned char length)
{
	uint8_t	i;
	switch (lEventType) {
	case HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE:
		ulSmartConfigFinished =	1;
		ucStopSmartConfig     =	1;
		break;
	case HCI_EVNT_WLAN_UNSOL_CONNECT:
		ulCC3000Connected = 1;
		puts_P(PSTR("HCI_EVNT_WLAN_UNSOL_CONNECT"));
		break;
	case HCI_EVNT_WLAN_UNSOL_DISCONNECT:
		ulCC3000Connected = 0;
		ulCC3000DHCP	  = 0;
		ulCC3000DHCP_configured	= 0;
		printOnce = 1;
		puts_P(PSTR("HCI_EVNT_WLAN_UNSOL_DISCONNECT"));
		break;
	case HCI_EVNT_WLAN_UNSOL_DHCP:
		// Notes:
		// 1) IP config	parameters are received	swapped
		// 2) IP config	parameters are valid only if status is OK,
		// i.e.	ulCC3000DHCP becomes 1

		// Only	if status is OK, the flag is set to 1 and the
		// addresses are valid.
		if ( *(data + NETAPP_IPCONFIG_MAC_OFFSET) == 0)	{
			printf_P(PSTR("IP: "));
			i = 3;
			do {
				printf("%u", (uint8_t)data[i]);
				putchar(i != 0 ? '.' : '\n');
			} while	(i-- !=	0);
			ulCC3000DHCP = 1;
			puts_P(PSTR("HCI_EVNT_WLAN_UNSOL_DHCP: success"));
		} else {
			ulCC3000DHCP = 0;
			puts_P(PSTR("HCI_EVNT_WLAN_UNSOL_DHCP: failed"));
		}
		break;
	case HCI_EVNT_WLAN_ASYNC_PING_REPORT: {
		netapp_pingreport_args_t *report = (netapp_pingreport_args_t *)data;
		printf_P(PSTR("Packets:	%lu/%lu\n"), report->packets_received, report->packets_sent);
		printf_P(PSTR("Round time: %lu(%lu, %lu)\n"), report->avg_round_time, report->min_round_time, report->max_round_time);
		break;
	}
	case HCI_EVENT_CC3000_CAN_SHUT_DOWN:
		OkToDoShutDown = 1;
		break;
	default:
		printf_P(PSTR("Event: %lX, %u\n"), lEventType, length);
	}
}

void scanNetworks()
{
#if 1
	uint32_t intv[16] = {2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000};
	int32_t	res = wlan_ioctl_set_scan_params(1, 20,	30, 2, 0x1FFF, -80, 0, 205, intv);
	printf_P(PSTR("WLAN scan params: %ld\n"), res);
#endif

	uint8_t	count =	0;
	_wlan_full_scan_results_args_t scanRes;
	do {
		res = wlan_ioctl_get_scan_results(1000,	(uint8_t *)&scanRes);
		if (count == 0)
			count =	scanRes.count;
		printf_P(PSTR("WLAN scan: %ld (No. %lu:	%lu, %u)\n"), res, count - scanRes.count, scanRes.status, scanRes.valid);
		if (scanRes.status != 1	|| !scanRes.valid)
			continue;
		printf_P(PSTR("Security: %u, name: "), scanRes.secMode);
		for (uint8_t i = 0; i <	scanRes.length;	i++)
			putchar(scanRes.name[i]);
		putchar('\n');
	} while	(scanRes.count - 1);
}

void init()
{
	tft.init();
	tft.setOrient(tft.Portrait);
	tft.setForeground(0x667F);
	tft.setBackground(0x0000);
	stdout = tftout(&tft);
	tft.setBGLight(true);

	DDRB |=	_BV(7);
	sei();
	printf_P(PSTR("Host driver version: %u\n"), DRIVER_VERSION_NUMBER);

	init_spi();
	puts_P(PSTR("SPI initialised."));

	wlan_init(CC3000_UsynchCallback, sendPatch, sendPatch, sendPatch, ReadWlanInterruptPin,	WlanInterruptEnable, WlanInterruptDisable, WriteWlanPin);
	puts_P(PSTR("WLAN initialised."));
	int32_t	res;

	wlan_start(0);
	puts_P(PSTR("WLAN started."));
	wlan_set_event_mask(HCI_EVENT_MASK);	// Mask	out all	non-required events from CC3000

#if 0
	puts_P(PSTR("wlan_ioctl_set_connection_policy(0, 0, 0)"));
	wlan_ioctl_set_connection_policy(0, 0, 0);
	puts_P(PSTR("wlan_ioctl_del_profile(255)"));
	wlan_ioctl_del_profile(255);
	/*while	(ulCC3000Connected == 1)
		_delay_us(10);*/
	puts_P(PSTR("wlan_smart_config_set_prefix((char*)aucCC3000_prefix)"));
	wlan_smart_config_set_prefix((char*)aucCC3000_prefix);
	/*wlan_smart_config_start(0);
	while (ulSmartConfigFinished ==	0)
		_delay_us(10);*/
	puts_P(PSTR("wlan_ioctl_set_connection_policy(1, 1, 1)"));
	wlan_ioctl_set_connection_policy(1, 1, 1);
	puts_P(PSTR("wlan_stop()"));
	wlan_stop();
	puts_P(PSTR("wlan_start(0)"));
	wlan_start(0);
#endif

	uint8_t	ver[2];
	res = nvmem_read_sp_version(ver);
	printf_P(PSTR("Read version: %ld / %u.%u\n"), res, ver[0], ver[1]);

	uint8_t	mac[6];// = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
#if 0
	res = nvmem_set_mac_address(mac);
	printf_P(PSTR("Set MAC:	%u\n"),	res);
	wlan_stop();
	puts_P(PSTR("WLAN stopped."));
	wlan_start(0);
	puts_P(PSTR("WLAN started."));
#endif

	res = nvmem_get_mac_address(mac);
	printf_P(PSTR("Get MAC:	%ld\n"), res);
	if (res	== 0) {
		printf_P(PSTR("MAC address: "));
		for (uint8_t i = 0; i != 6; i++) {
			printf_P(PSTR("%02X"), mac[i]);
			putchar(i != 5 ? ':' : '\n');
		}
	}

#if 0
	res = wlan_disconnect();
	printf_P(PSTR("WLAN disconnect:	%ld\n"), res);
#endif

	scanNetworks();

#if 0
	char ssid[] = "Network";
	res = wlan_connect(WLAN_SEC_UNSEC, ssid, strlen(ssid), 0, 0, 0);
#endif
#if 1
	char ssid[] = "ZZFNB00000017_Network";
	char key[] = "f3ei-zeb6-m35o";
	res = wlan_connect(WLAN_SEC_WPA2, ssid, strlen(ssid), 0, (uint8_t *)key, strlen(key));
#endif
#if 0
	uint8_t	bssid[6] = {0x00, 0x1F,	0xA3, 0x1F, 0xC0, 0xF7};
	res = wlan_connect(WLAN_SEC_UNSEC, 0, 0, bssid,	0, 0);
#endif
	printf_P(PSTR("WLAN connect: %ld\n"), res);

	ucStopSmartConfig = 0;
}

int main()
{
	init();

	uint8_t	ping = 1;
start:
	//printf_P(PSTR("WLAN status: %ld\n"), wlan_ioctl_statusget());
	//_delay_ms(1000);
	if (ulCC3000DHCP && ping) {
		ping = 0;
		uint8_t	ip[4] =	{216, 58, 208, 78};
		printf_P(PSTR("Ping send: %ld\n"), netapp_ping_send((uint32_t *)ip, 10,	32, 1000));
		/*netapp_ping_report();
		puts_P(PSTR("Ping report requested."));*/
	}
	goto start;
	return 1;
}
