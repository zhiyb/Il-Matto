#include <avr/interrupt.h>
#include "led.h"

using namespace led;

int main(void)
{
	led::init();
	sei();
	for (uint8_t j = 0; j < LED_H; j++)
		for (uint8_t k = 0; k < LED_W / 8; k++) {
			led::buff[j][k][led::BuffRed] = 0x55;
			led::buff[j][k][led::BuffGreen] = 0xAA;
		}
	led::fill(None);
	//led::drawChar(3, 3, 1, Red << Foreground | Orange << Background, 'T');
	//led::drawChar(12, 12, 2, Red << Foreground | None << Background, 'E');
	//led::drawChar(30, 2, 3, None << Foreground | Red << Background, 'S');
	led::drawString(2, 0, 2, Red << Foreground | None << Background, "Hello");
	led::drawString(2, 16, 2, Green << Foreground | None << Background, "World");
	led::drawEllipse(0, 0, 63, 31, Orange);
	//led::fill();
	//buff[3][3][Green] = 0x00;
	while (1);
	return 1;
}
