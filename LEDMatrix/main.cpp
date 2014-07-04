#include <avr/interrupt.h>
#include <util/delay.h>
#include "display.h"
#include "sd.h"
#include "ascii.h"

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
	class sdhw_t sd;
	uint8_t row, err;
	char str[11];
	sei();

	drawString(2, 0, 2, Red << Foreground | None << Background, \
			"Hello");
	drawString(2, 16, 2, Green << Foreground | None << Background, \
			"World");
	drawEllipse(0, 0, 63, 31, Orange);
	update();
	_delay_ms(500);

start:
	fill(None);
	row = 0;
	if (!sd.detect()) {
		update();
		goto start;
	}
	drawString(0, row, 1, Green << Foreground, "SDCard.");
	row += FONT_H;

	if (sd.writeProtected()) {
		drawString(0, row, 1, Red << Foreground, "Protected.");
		row += FONT_H;
	}
	if ((err = sd.init()) != 0x00) {
		drawString(0, row, 1, Red << Foreground, "Failed.");
		goto start;
	}
	sprintf(str, "%u MB", (uint16_t)(sd.size() / 1024));
	drawString(0, row, 1, Orange << Foreground, str);
	row += FONT_H;
	update();

	while (sd.detect());
	goto start;

	while (1);
	return 1;
}
