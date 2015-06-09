#include <avr/io.h>
#include "cc3000.h"
#include "Hardware/spi.h"

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

void cc3000_init(tWlanCB sWlanCB)
{
	spi_init();
	wlan_init(sWlanCB, sendPatch, sendPatch, sendPatch, ReadWlanInterruptPin, WlanInterruptEnable, WlanInterruptDisable, WriteWlanPin);
}
