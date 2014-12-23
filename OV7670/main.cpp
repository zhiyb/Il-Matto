#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <tft.h>
#include <ov7670.h>

void init(void)
{
	OV7670::init();
	OV7670::reset();
	OV7670::write(0x12, 0x80);
	_delay_ms(1);
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
	for (uint8_t i = 0; i < 0xC0; i++)
		printf("%02X/%02X\t", i, OV7670::read(i));
	while (1);
	goto start;

	return 1;
}
