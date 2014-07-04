#ifndef USB_H
#define USB_H

#define USB_DDR		DDRD
#define USB_PORT	PORTD
#define USB_DMINUS	_BV(PD2)
#define USB_DPLUS	_BV(PD3)

static inline void usbDisconnect(void)
{
	USB_DDR |= USB_DMINUS;
	USB_PORT &= ~USB_DMINUS;
}

#endif
