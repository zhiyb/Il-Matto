#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <tft.h>
#include <rfm12_config.h>
#include <rfm12.h>
#ifdef RTOSPORT
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#endif

#define RFM12B_TX

#ifdef RTOSPORT
void rfm12_tick_task(void *param)
{
loop:
	taskENTER_CRITICAL();
	{
		rfm12_tick();
	}
	taskEXIT_CRITICAL();
	vTaskDelay(configTICK_RATE_HZ / 1000);
	goto loop;
}

void rfm12_layer(void *param)
{
	uint16_t cnt = 0;
loop:
#ifdef RFM12B_TX
	static uint8_t teststr[] = "Hello, world!\r\n";
	printf_P(PSTR("%02x, "), rfm12_tx(sizeof(teststr), 0xaa, teststr));
	printf_P(PSTR("sent: %u\n"), cnt++);
	vTaskDelay(configTICK_RATE_HZ * 100 / 1000);
#else
	uint8_t recv;
	while (xQueueReceive(rfm12_rx_queue, &recv, portMAX_DELAY) != pdTRUE);
	//if (recv != STATUS_COMPLETE)
	//	goto loop;
	uint8_t len = rfm12_rx_len();
	uint8_t type = rfm12_rx_type();
	uint8_t *buffer = rfm12_rx_buffer();
	printf_P(PSTR("Received %u: 0x%02x(%u)\n"), cnt++, type, len);
	puts((char *)buffer);
	rfm12_rx_clear();
#endif
	goto loop;
}
#endif

void init()
{
	tft::init();
	stdout = tft::devout();
	rfm12_init();
	sei();
}

int main()
{
	init();
	tft::setBGLight(true);

	puts("Initialised.");
#ifdef RTOSPORT
	xTaskCreate(rfm12_int_task, "RFM12INT", configMINIMAL_STACK_SIZE, \
			NULL, configMAX_PRIORITIES, NULL);
	xTaskCreate(rfm12_tick_task, "RFM12TICK", configMINIMAL_STACK_SIZE, \
			NULL, tskIDLE_PRIORITY, NULL);
	xTaskCreate(rfm12_layer, "LAY_RFM12", configMINIMAL_STACK_SIZE * 2, \
			NULL, tskIDLE_PRIORITY + 1, NULL);
	puts("Tasks created.");
	vTaskStartScheduler();
#else
#ifdef RFM12B_TX
	static uint8_t teststr[] = "Hello, world!\r\n";
	uint16_t cnt = 0;
	for (;;) {
		printf("%02x, ", rfm12_tx(sizeof(teststr), 0xaa, teststr));
		rfm12_start_tx(0xaa, sizeof(teststr));
		rfm12_tick();
		printf("sent: %u\n", cnt++);
		for (uint16_t i = 0; i < 1000; i++) {
			_delay_us(500);
			rfm12_tick();
		}
	}
#else
	for (;;) {
		rfm12_tick();
		while (rfm12_rx_status() != STATUS_COMPLETE)
			rfm12_tick();
		printf("Received: ");
		uint8_t len = rfm12_rx_len();
		uint8_t type = rfm12_rx_type();
		uint8_t *buffer = rfm12_rx_buffer();
		printf("length %u, type 0x%02x\n", len, type);
		puts((char *)buffer);
		rfm12_rx_clear();
	}
#endif
#endif

	return 1;
}
