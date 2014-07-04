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

void init(void)
{
	usbDisconnect();
	disp.init();
	stdout = displayOut();
}

int main(void)
{
	init();
	class sdhw_t sd;
	uint8_t err;
	sei();

	disp.setColour(Red);
	disp.drawString(2, 0, 2,"Hello");
	disp.setColour(Green);
	disp.drawString(2, 16, 2, "World");
	disp.setColour(Orange);
	disp.drawEllipse(0, 0, 63, 31);
	disp.update();
	_delay_ms(500);

start:
	disp.setColour(None);
	disp.clear();
	if (!sd.detect()) {
		disp.update();
		goto start;
	}
	disp.setColour(Green);
	puts("SDCard.");

	if (sd.writeProtected()) {
		disp.setColour(Red);
		puts("Protected.");
	}
	if ((err = sd.init()) != 0x00) {
		disp.setColour(Red);
		puts("Failed.");
		goto ret;
	}
	disp.setColour(Orange);
	printf("%u MB\n", (uint16_t)(sd.size() / 1024));

ret:
	disp.update();
	while (sd.detect());
	goto start;

	while (1);
	return 1;
}
