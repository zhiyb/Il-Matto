// Author: Yubo Zhi (normanzyb@gmail.com)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <macros.h>
#include <colours.h>
#include "FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>
#include <rgbled.h>
#include <tft.h>

#define DELAY_ANI	(configTICK_RATE_HZ / 64)
#define DELAY_TFT	(0)

using namespace colours::b32;

static const uint32_t pilots[] = {
#if 0
	Black,
	DarkRed, DarkGreen, DarkBlue,
	LightRed, LightGreen, LightBlue,
	DarkYellow, DarkCyan, DarkMagenta,
	LightYellow, LightCyan, LightMagenta,
	White, Grey, Black,
#elif 1
	Red, Orange, Yellow, Chartreuse,
	Green, SpringGreen, Cyan, Azure,
	Blue, Violet, Magenta, Pink,
#endif
};
static const uint8_t num_pilots = sizeof(pilots) / sizeof(pilots[0]);
static const uint16_t total_steps = num_pilots * RGBLED_NUM * 2;

uint32_t ani(uint16_t s)
{
	static const uint8_t itvl = RGBLED_NUM;
	uint8_t step = s / itvl;
	uint8_t i = s - itvl * step;
	uint8_t d = step & 1;
	uint8_t c = step / 2;

	uint32_t clr = pilots[c];
	uint16_t ci = 0;

	ci = 256 * i / itvl;
	if (d)
		ci = 256 - ci;

	uint8_t r = (uint16_t)RED_888(clr) * ci / 256;
	uint8_t g = (uint16_t)GREEN_888(clr) * ci / 256;
	uint8_t b = (uint16_t)BLUE_888(clr) * ci / 256;

	return COLOUR_888(r, g, b);
}

void LEDTask(void *param)
{
	uint16_t s = 0;
	uint8_t n = RGBLED_NUM;
	uint8_t i;
	for (i = 0; i < n; i++)
		rgbLED[i] = ani(s++);

	TickType_t xLastWakeTime = 0;	// Same starting tick reference
	for (;;) {
		rgbLED_refresh();
		vTaskDelayUntil(&xLastWakeTime, DELAY_ANI);

		for (i = 0; i < n - 1; i++)
			rgbLED[i] = rgbLED[i + 1];
		rgbLED[i] = ani(s++);
		s = s % total_steps;
	}
}

void TFTTask(void *param)
{
	using namespace tft;
	background = 0x0000;
	foreground = 0x667f;
	zoom = 1;
	puts("TFT task initialised.\n");
	zoom = 1;
	uint16_t yy = y;
loop:
	y = yy;
	for (uint8_t i = 0; i < RGBLED_NUM; i++) {
		foreground = COLOUR_565_888(rgbLED[i]);
		printf("%06lx\n", rgbLED[i]);
	}
	vTaskDelay(DELAY_TFT);
	goto loop;
}

void init()
{
	tft::init();
	stdout = tft::devout();
	rgbLED_init();
	rgbLED_refresh();
	sei();
	tft::setOrient(tft::Portrait);
	tft::setBGLight(true);
}

int main()
{
	init();

	puts("Creating tasks...");
	xTaskCreate(TFTTask, "TFT", configMINIMAL_STACK_SIZE * 2, NULL, \
			tskIDLE_PRIORITY, NULL);
	xTaskCreate(LEDTask, "LEDTask", configMINIMAL_STACK_SIZE, NULL, \
			tskIDLE_PRIORITY + 1, NULL);
	vTaskStartScheduler();

	return 1;
}
