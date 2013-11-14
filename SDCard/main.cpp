#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "tft.h"
#include "sd.h"

void init(void)
{
	tft.init();
	tft /= tft.Portrait;
	tft.clean();
	stdout = tftout();
	tft++;
}

int main(void)
{
	init();

start:
	tft.clean();
	puts("*** SD Card library testing ***");
	puts("Waiting for SD Card insert...");
	while (!sd.detect());
	puts("SD Card detected!");
	if (sd.writeProtected())
		puts("SD Card is write-protected!");
	else
		puts("SD Card is not write-protected!");
	puts("Initialising SD Card...");
	printf("Return value: 0x%02X\n", sd.init());
	for (uint8_t r = 0; r < 5; r++)
		printf("Response value index %d is 0x%02X\n", r, sd.res[r]);
	puts("Remove SD Card to run the test again...");
	while (sd.detect());
	goto start;

	return 1;
}
