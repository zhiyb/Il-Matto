#include <avr/io.h>
#include <avr/interrupt.h>
#include <tft.h>
#include <spi.h>
#include <wlan.h>
#include <hci.h>
#include <host_driver_version.h>

#define NETAPP_IPCONFIG_MAC_OFFSET	(20)
#define HCI_EVENT_MASK	(HCI_EVNT_WLAN_KEEPALIVE | HCI_EVNT_WLAN_UNSOL_INIT | HCI_EVNT_WLAN_ASYNC_PING_REPORT)

tft_t tft;

volatile unsigned long ulSmartConfigFinished, ulCC3000Connected,ulCC3000DHCP,
			OkToDoShutDown, ulCC3000DHCP_configured;
volatile unsigned char ucStopSmartConfig;

unsigned char printOnce = 1;

long ReadWlanInterruptPin(void)
{
	return SPI_R & CC3000_INT;
}

void WlanInterruptEnable()
{
	PCICR |= _BV(PCIE3);
	PCMSK3 = CC3000_INT;
	//SpiResumeSpi();
	//P2IES |= BIT3;
	//P2IE |= BIT3;
}

void WlanInterruptDisable()
{
	PCMSK3 &= ~CC3000_INT;
}

void WriteWlanPin(unsigned char val)
{
	if (val)
		SPI_W |= CC3000_EN;
	else
		SPI_W &= ~CC3000_EN;
}

char *sendBootLoaderPatch(unsigned long *Length)
{
	*Length = 0;
	return NULL;
}

char *sendDriverPatch(unsigned long *Length)
{
	*Length = 0;
	return NULL;
}

char *sendWLFWPatch(unsigned long *Length)
{
	*Length = 0;
	return NULL;
}

//*****************************************************************************
//
//! CC3000_UsynchCallback
//!
//! @param  lEventType   Event type
//! @param  data   
//! @param  length   
//!
//! @return none
//!
//! @brief  The function handles asynchronous events that come from CC3000  
//!         device and operates a LED1 to have an on-board indication
//
//*****************************************************************************
void CC3000_UsynchCallback(long lEventType, char * data, unsigned char length)
{
	switch (lEventType) {
	case HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE:
        	ulSmartConfigFinished = 1;
        	ucStopSmartConfig     = 1;
        	break;
        case HCI_EVNT_WLAN_UNSOL_CONNECT:
        	ulCC3000Connected = 1;
        	//turnLedOn(7);
        	puts_P(PSTR("HCI_EVNT_WLAN_UNSOL_CONNECT"));
        	break;
        case HCI_EVNT_WLAN_UNSOL_DISCONNECT:
        	ulCC3000Connected = 0;
        	ulCC3000DHCP      = 0;
        	ulCC3000DHCP_configured = 0;
        	printOnce = 1;
        	//turnLedOff(7);
        	//turnLedOff(8);
        	puts_P(PSTR("HCI_EVNT_WLAN_UNSOL_DISCONNECT"));
        	break;
        case HCI_EVNT_WLAN_UNSOL_DHCP:
	        // Notes: 
        	// 1) IP config parameters are received swapped
        	// 2) IP config parameters are valid only if status is OK, 
        	// i.e. ulCC3000DHCP becomes 1
      
	        // Only if status is OK, the flag is set to 1 and the 
	        // addresses are valid.
	        if ( *(data + NETAPP_IPCONFIG_MAC_OFFSET) == 0) {
            		//sprintf((char*)pucCC3000_Rx_Buffer,"IP:%d.%d.%d.%d\f\r", data[3],data[2], data[1], data[0]);
            		printf_P(PSTR("IP:%d.%d.%d.%d\n"), data[3],data[2], data[1], data[0]);
            		ulCC3000DHCP = 1;
            		//turnLedOn(8);
        		puts_P(PSTR("HCI_EVNT_WLAN_UNSOL_DHCP: On"));
        	} else {
            		ulCC3000DHCP = 0;
            		//turnLedOff(8);
        		puts_P(PSTR("HCI_EVNT_WLAN_UNSOL_DHCP: Off"));
        	}
        	break;
        case HCI_EVENT_CC3000_CAN_SHUT_DOWN:
        	OkToDoShutDown = 1;
	}
}

void init()
{
	tft.init();
	tft.setOrient(tft.Portrait);
	tft.setForeground(0x667F);
	tft.setBackground(0x0000);
	stdout = tftout(&tft);
	tft.setBGLight(true);

	DDRB |= _BV(7);
	sei();
	printf_P(PSTR("Host driver version: %u\n"), DRIVER_VERSION_NUMBER);
	init_spi();
	puts_P(PSTR("SPI initialised."));
	wlan_init(CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch, sendBootLoaderPatch, ReadWlanInterruptPin, WlanInterruptEnable, WlanInterruptDisable, WriteWlanPin);
	puts_P(PSTR("WLAN initialised."));
	wlan_start(0);
	puts_P(PSTR("WLAN started."));

	// Mask out all non-required events from CC3000
	wlan_set_event_mask(HCI_EVENT_MASK);
}

int main()
{
	init();
start:
	goto start;
	return 1;
}
