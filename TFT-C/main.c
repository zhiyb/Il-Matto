#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <tft.h>

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft_init();
	tft_setOrient(FlipLandscape);
	tft_setBackground(0x667F);
	tft_setForeground(0x0000);
	tft_clean();
	stdout = tftout();
	tft_setBGLight(1);
}

int main(void)
{
	init();

start:
	tft_clean();
	tft_setZoom(1);
	puts("*** TFT library testing ***");
	puts("STDOUT output, orientation, FG/BG colour, BG light");
	tft_setZoom(3);
	puts("Font size test");
	tft_setZoom(1);
	tft_setXY(300, 38);
	puts("Change text position & word warp test");
	tft_frame(115, 56, 200, 10, 2, 0xF800);
	puts("Draw frame test");
	tft_rectangle(118, 68, 180, 6, 0x07E0);
	puts("Draw rectangle test");
	tft_point(120, 76, 0x001F);
	tft_point(122, 76, 0x001F);
	tft_point(124, 76, 0x001F);
	tft_point(126, 76, 0x001F);
	tft_point(128, 76, 0x001F);
	tft_point(130, 76, 0x001F);
	puts("Draw points test");
	tft_line(200, 100, 300, 200, 0xF800);
	tft_line(300, 210, 200, 110, 0x001F);
	tft_line(200, 200, 300, 100, 0xF800);
	tft_line(300, 110, 200, 210, 0x001F);

	tft_line(100, 100, 300, 200, 0xF800);
	tft_line(300, 210, 100, 110, 0x001F);
	tft_line(100, 200, 300, 100, 0xF800);
	tft_line(300, 110, 100, 210, 0x001F);

	tft_line(200, 0, 300, 200, 0xF800);
	tft_line(300, 210, 200, 10, 0x001F);
	tft_line(200, 200, 300, 0, 0xF800);
	tft_line(300, 10, 200, 210, 0x001F);

	tft_line(100, 150, 300, 150, 0xF800);
	tft_line(300, 160, 100, 160, 0x001F);
	tft_line(250, 0, 250, 200, 0xF800);
	tft_line(260, 200, 270, 0, 0x001F);
	puts("Draw lines test");
	while (1);
	goto start;

	return 1;
}
