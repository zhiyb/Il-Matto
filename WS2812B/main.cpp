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

#define DELAY_REFRESH	(configTICK_RATE_HZ / 32)
#define DELAY_ANI	(configTICK_RATE_HZ / 256)
#define DELAY_TFT	(0)
#define MIDDLE		64

using namespace colours::b32;

void RefreshTask(void *param)
{
	for (;;) {
		vTaskDelay(DELAY_REFRESH);
		rgbLED_refresh();
	}
}

void LEDTask(void *param)
{
	const static uint32_t colours[] = {
#if 0
		Black,
		DarkRed, DarkGreen, DarkBlue,
		LightRed, LightGreen, LightBlue,
		DarkYellow, DarkCyan, DarkMagenta,
		LightYellow, LightCyan, LightMagenta,
		White, Grey, Black,
#endif
		Red, Orange, Yellow, Chartreuse,
		Green, SpringGreen, Cyan, Azure,
		Blue, Violet, Magenta, Pink,
	};
	const static uint8_t clrs = sizeof(colours) / sizeof(colours[0]);

	uint8_t n = (uint8_t)(uint16_t)param;
	uint8_t step = 255 * (n % (RGBLED_NUM / 2)) * 2 / RGBLED_NUM;
	uint8_t dir = n >= RGBLED_NUM / 2;
	uint8_t clr = 0;
	TickType_t xLastWakeTime = 0;	// Same starting tick reference
loop:
	do {
		do {
			uint16_t i = dir ? 255 - step : step;
			if (i < 128)
				i = i * MIDDLE / 128;
			else
				i = MIDDLE + (i - 128) * (255 - MIDDLE) / 127;
			uint32_t colour = *(colours + clr);
			uint8_t r = RED_888(colour) * i / 255;
			uint8_t g = GREEN_888(colour) * i / 255;
			uint8_t b = BLUE_888(colour) * i / 255;
			rgbLED[n] = COLOUR_888(r, g, b);
			vTaskDelayUntil(&xLastWakeTime, DELAY_ANI);
		} while (++step);
	} while (++dir != 2);
	dir = 0;
	clr = clr == clrs - 1 ? 0 : clr + 1;
	goto loop;
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
	xTaskCreate(RefreshTask, "Refresh", configMINIMAL_STACK_SIZE, NULL, \
			configMAX_PRIORITIES, NULL);
	xTaskCreate(TFTTask, "TFT", configMINIMAL_STACK_SIZE * 2, NULL, \
			tskIDLE_PRIORITY, NULL);
	uint16_t i;
	for (i = 0; i != 8; i++)
		xTaskCreate(LEDTask, "LEDTask", configMINIMAL_STACK_SIZE, \
				(void *)i, tskIDLE_PRIORITY + 1, NULL);
	vTaskStartScheduler();

	return 1;
}
