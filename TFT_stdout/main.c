#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "tft.h"

void init(void)
{
	TFT_init();
	stdout = tftout();
	stderr = tftout();
}

int main(void)
{
	init();

	puts("*** TFT library test***");
	while (1) {
		printf("Value of d: %d\n", rand());
		_delay_ms(100);
	}

	return 1;
}
