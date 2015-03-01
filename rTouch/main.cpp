#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <tft.h>
#include <rtouch.h>
#include <eemem.h>

tft_t tft;
rTouch touch(&tft);

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft.init();
	tft.setOrient(tft.Portrait);
	tft.setBackground(0x0000);
	tft.setForeground(0x667F);
	stdout = tftout(&tft);
	touch.init();
	tft.setBGLight(true);
	touch.calibrate();
	tft.clean();
	eeprom_first_done();
}

int main(void)
{
	init();

start:
	tft.clean();
	tft.setZoom(2);
	puts("*** Touch ***");

	uint16_t clr = 0xFFFF;
loop:
	if (touch.detect()) {
		rTouch::coord_t res = touch.read();
		if (!touch.detect())
			goto loop;
		if (res.x <= -20) {
#if 1
			int16_t spl = tft.height() / 7;
			if (res.y < spl)
				clr = 0xF800;
			else if (res.y < spl * 2)
				clr = 0x07E0;
			else if (res.y < spl * 3)
				clr = 0x001F;
			else if (res.y < spl * 4)
				clr = 0xFFE0;
			else if (res.y < spl * 5)
				clr = 0x07FF;
			else if (res.y < spl * 6)
				clr = 0xF81F;
			else
				clr = 0xFFFF;
#else
			uint8_t mv = res.y * 16 / tft.height();
			clr = (0x001F << mv) | (((uint32_t)0x001F << mv) >> 16);
#endif
		} else if (res.x > (int16_t)(tft.width() + 20))
			tft.clean();
		else
			tft.point(res.x, res.y, clr);
			//printf("TE: %u\t%u\n", res.x, res.y);
	}
	goto loop;
	goto start;

	return 1;
}
