#include <avr/interrupt.h>
#include <util/delay.h>
#include "display.h"
#include "sd.h"
#include "ascii.h"

#define USB_DDR		DDRD
#define USB_PORT	PORTD
#define USB_DMINUS	_BV(PD2)
#define USB_DPLUS	_BV(PD3)

void usbDisconnect(void)
{
	USB_DDR |= USB_DMINUS;
	USB_PORT &= ~USB_DMINUS;
}

using namespace display;

int main(void)
{
	usbDisconnect();
	disp.init();
	class sdhw_t sd;
	uint8_t row, err;
	char str[11];
	sei();

	disp.drawString(2, 0, 2, Red,"Hello");
	disp.drawString(2, 16, 2, Green, "World");
	disp.drawEllipse(0, 0, 63, 31, Orange);
	disp.update();
	_delay_ms(500);

start:
	disp.fill(None);
	row = 0;
	if (!sd.detect()) {
		disp.update();
		goto start;
	}
	disp.drawString(0, row, 1, Green, "SDCard.");
	row += FONT_H;

	if (sd.writeProtected()) {
		disp.drawString(0, row, 1, Red, "Protected.");
		row += FONT_H;
	}
	if ((err = sd.init()) != 0x00) {
		disp.drawString(0, row, 1, Red, "Failed.");
		goto start;
	}
	sprintf(str, "%u MB", (uint16_t)(sd.size() / 1024));
	disp.drawString(0, row, 1, Orange, str);
	row += FONT_H;
	disp.update();

	while (sd.detect());
	goto start;

	while (1);
	return 1;
}
