#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <tft.h>
#include <ov7670.h>

void init(void)
{
	OV7670::init();
	tft::init();
	tft::setOrient(tft::Portrait);
	tft::foreground = 0x667f;
	tft::background = 0x0000;
	tft::clean();
	stdout = tft::devout();
	tft::setBGLight(true);
}

#if 0
void settings(void)
{
	static struct regval_list set[] = {
	{0x15, 0x02},	// VSYNC negative

	{0x3E, 0x19},
	{0x72, 0x11},
	{0x73, 0xF1},
	{0x17, 0x16},
	{0x18, 0x04},
	{0x32, 0x24},
	{0x19, 0x02},
	{0x1A, 0x7A},
	{0x03, 0x0A},

#if 1
	{0x12, 0x04},	// COM7
	{0x8C, 0x00},	// RGB444
	{0x40, 0xD0},	// COM15
	{0x14, 0x6A},	// COM9
#endif

#if 1
	//write(0x14, 0x6A);	// Gain ceiling
	{0x4F, 0xB3},
	{0x50, 0xB3},
	{0x51, 0x00},
	{0x52, 0x3D},
	{0x53, 0xA7},
	{0x54, 0xE4},
	{0x3D, 0x40},	// COM13: UV saturation auto adjustment
#endif

	{0xFF, 0xFF},	// END
	};
	OV7670::writeArray(set);

	_delay_ms(10);
}
#endif

int main(void)
{
	init();

start:
	tft::clean();
	tft::zoom = 1;
	puts("*** OV7670 ***");
	//settings();
	//OV7670::startCapture();
	for (uint8_t i = 0; i < 0x80; i++)
		printf("%02X/%02X\t", i, OV7670::read(i));
	tft::setOrient(tft::Flipped | tft::Landscape);

	while (1) {
		tfthw::all();
		tfthw::memWrite();
		OV7670::enableFIFO(true);
		for (uint8_t y = 0; y < 240; y++)
			for (uint16_t x = 0; x < 320; x++) {
				uint16_t c = (uint16_t)OV7670::readFIFO() << 8;
				c |= OV7670::readFIFO();
				tfthw::write16(c);
			}
		OV7670::enableFIFO(false);
		//_delay_ms(1000);
	}

	tft::setOrient(tft::Portrait);
	tft::x = 0;
	tft::y = 0;
	while (1);
	goto start;

	return 1;
}
