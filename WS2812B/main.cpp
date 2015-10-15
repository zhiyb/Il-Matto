// Author: Yubo Zhi (normanzyb@gmail.com)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <macros.h>
#include "FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>
#include <rgbled.h>
#include <tft.h>

#define DELAY_REFRESH	(configTICK_RATE_HZ / 32)
#define DELAY_ANI	(configTICK_RATE_HZ / 256)
#define DELAY_TFT	(0)
#define MIDDLE		64

const static uint8_t colours[][3] = {
	{0xff, 0x00, 0x00,},
	{0xff, 0xff, 0x00,},
	{0x00, 0xff, 0x00,},
	{0x00, 0xff, 0xff,},
	{0x00, 0x00, 0xff,},
	{0xff, 0x00, 0xff,},
};

void RefreshTask(void *param)
{
	for (;;) {
		vTaskDelay(DELAY_REFRESH);
		rgbLED_refresh();
	}
}

void LEDTask(void *param)
{
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
			uint8_t r = (uint16_t)colours[clr][0] * i / 255;
			uint8_t g = (uint16_t)colours[clr][1] * i / 255;
			uint8_t b = (uint16_t)colours[clr][2] * i / 255;
			rgbLED[n] = COLOUR_888(r, g, b);
			vTaskDelayUntil(&xLastWakeTime, DELAY_ANI);
		} while (++step);
	} while (++dir != 2);
	dir = 0;
	clr = clr == sizeof(colours) / 3 - 1 ? 0 : clr + 1;
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
