#include <avr/interrupt.h>
#include <util/delay.h>
#include "led.h"

int main(void)
{
	led::init();
	return 0;
}
