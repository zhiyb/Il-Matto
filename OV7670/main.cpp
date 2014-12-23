#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <tft.h>
#include <ov7670.h>

class OV7670hw ov;

void init(void)
{
	ov.init();
	tft.init();
	tft /= tft.Portrait;
	tft.setForeground(0x667F);
	tft.setBackground(0x0000);
	tft.clean();
	stdout = tftout();
	tft++;
}

int main(void)
{
	init();

start:
	tft.clean();
	tft *= 1;
	puts("*** OV7670 ***");
	ov.write(0x12, 0x80);
	for (uint8_t i = 0; i < 0xC0; i++)
		printf("%02X/%02X\t", i, ov.read(i));
	while (1);
	goto start;

	return 1;
}
