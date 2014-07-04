#include <avr/interrupt.h>
#include "display.h"

#define USB_DDR		DDRD
#define USB_PORT	PORTD
#define USB_DMINUS	_BV(PD2)
#define USB_DPLUS	_BV(PD3)

using namespace display;

void usbDisconnect(void)
{
	USB_DDR |= USB_DMINUS;
	USB_PORT &= ~USB_DMINUS;
}

int main(void)
{
	usbDisconnect();
	display::init();
	sei();
	for (uint8_t j = 0; j < LED_H; j++)
		for (uint8_t k = 0; k < LED_W / 8; k++) {
			buff[j][k][BuffRed] = 0x55;
			buff[j][k][BuffGreen] = 0xAA;
		}
	fill(None);
	drawString(2, 0, 2, Red << Foreground | None << Background, "Hello");
	drawString(2, 16, 2, Green << Foreground | None << Background, "World");
	drawEllipse(0, 0, 63, 31, Orange);
	//fill();
	//buff[3][3][Green] = 0x00;
	while (1);
	return 1;
}
