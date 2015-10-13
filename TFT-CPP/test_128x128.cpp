#include <util/delay.h>
#include <stdio.h>
#include <tft.h>
#include <FreeRTOS.h>
#include <task.h>

extern void test(void)
{
	using namespace tft;
	setOrient(Portrait);
	background = 0x667f;
	foreground = 0x0000;
	setBGLight(true);

start:
	clean();
	zoom = 1;
	puts("*** TFT testing ***");
	puts("STDOUT output, orientation, FG/BG colour, BG light");
	zoom = 2;
	puts("Font size");
	zoom = 1;
	x = 64;
	y = 64;
	puts("Text positioning & word warp");
	frame(12, 48, 24, 12, 2, 0xf800);
	puts("Frame.");
	rectangle(40, 50, 12, 18, 0x07e0);
	puts("Rectangle.");
	point(12, 64, 0x001F);
	point(14, 64, 0x001F);
	point(16, 64, 0x001F);
	point(18, 64, 0x001F);
	point(20, 64, 0x001F);
	point(22, 64, 0x001F);
	puts("Points.");
	line(0, 0, 127, 127, 0xf800);
	line(0, 127, 127, 0, 0x001f);
	line(0, 64, 127, 64, 0xf800);
	line(64, 0, 64, 127, 0x001f);
	line(0, 64 + 32, 127, 64 + 32, 0xf81f);
	line(64 + 32, 0, 64 + 32, 127, 0xf81f);
	puts("Lines.");

#ifdef TFT_VERTICAL_SCROLLING
	tft::setVerticalScrollingArea(64, 32);
	uint16_t max = tft::vsMaximum() - 32;
	uint16_t v = 64;
loop:
	setVerticalScrolling(v);
	if (max == ++v)
		v = 64;
	vTaskDelay(50);
#else
loop:
#endif
	goto loop;
	goto start;
}
