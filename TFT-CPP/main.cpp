#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <tft.h>
#ifdef TFT_READ_AVAILABLE
#include "capture.h"
#endif
#include "FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>

extern void test();
void TFTTask(void *param);
void LEDTask(void *param);

void init()
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft::init();
#ifdef TFT_READ_AVAILABLE
	capture::init();
	capture::enable();
#endif
	stdout = tft::devout();
	sei();
}

void TFTTask(void *param)
{
	for (;;)
		test();
}

void LEDTask(void *param)
{
	for (;;) {
		PINB |= 0x80;
		vTaskDelay(500);
	}
}

int main()
{
	init();

	xTaskCreate(TFTTask, "TFT Task", configMINIMAL_STACK_SIZE * 4, NULL, \
			tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(LEDTask, "LED Task", configMINIMAL_STACK_SIZE, NULL, \
			tskIDLE_PRIORITY + 1, NULL);
	vTaskStartScheduler();

	return 1;
}
