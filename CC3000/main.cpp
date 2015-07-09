#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <tft.h>
#include <eemem.h>
#include <rtouch.h>
#include <portraitlist.h>
#include "menu.h"
#include "event.h"

#include <cc3000.h>
#include <wlan.h>
#include <hci.h>
#include <nvmem.h>

tft_t tft;
static adcRequest_t adcReq;
rTouch touch(&tft, &adcReq);
static PortraitList l(&tft);

// Simple Config Prefix
//static const char aucCC3000_prefix[] =	{'T', 'T', 'T'};

ISR(ADC_vect, ISR_NOBLOCK)
{
	adcReq.isr();
}

void init()
{
	DDRB |=	_BV(7);
	PORTB |= _BV(7);

	tft.init();
	tft.setOrient(tft.Portrait);
	tft.setForeground(0x667F);
	tft.setBackground(0x0000);
	stdout = tftout(&tft);
	touch.init();
	sei();

	tft.setBGLight(true);
	touch.calibrate();
	eepromFirstDone();

	tft.clean();
	tft.setZoom(2);
	event::init();
	cc3000_init(event::usynchCallback);
	menu::wlan::reset::func(false);
	return;

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
}

int main()
{
	init();
	tft.setForeground(0x0000);

	l.refresh();
	l.setRootItem(&menu::root::item);
	l.display(&menu::root::item);

	for (;;)
		l.pool(&touch);
	return 1;

#if 0
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
#endif
}
