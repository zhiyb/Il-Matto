#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <tft.h>
#include <colours.h>
#include <rfm12_config.h>
#include <rfm12.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>

static volatile struct counter_t {
	uint32_t tx, rx;
} packages, bytes, pPkgs, pBytes;
static uint8_t rxLength, rxType;
static char rxBuffer[RFM12_RX_BUFFER_SIZE];

void rfm12_tick_task(void *param)
{
loop:
	rfm12_tick_critical();
	vTaskDelay(configTICK_RATE_HZ * 50 / 1000);
	goto loop;
}

void rfm12_layer(void *param)
{
	packages.tx = 0;
	packages.rx = 0;
	bytes.tx = 0;
	bytes.rx = 0;
	rxLength = 0;
loop:
	static uint8_t teststr[] = "Hello, world!\nFrom Norman Zhi, normanzyb@gmail.com\nElectronic Engineering 3rd year.\nyz39g13 @ University of Southampton\n";
	rfm12_tx(sizeof(teststr), 0xaa, teststr);
	// Trigger transmission now for higher throughput
	rfm12_tick_critical();
	packages.tx++;
	bytes.tx += sizeof(teststr);
receive:
	uint8_t recv;
	while (xQueueReceive(rfm12_queue, &recv, portMAX_DELAY) != pdTRUE);
	switch (recv) {
	case RFM12_QUEUE_TX:
		goto loop;	// Send next one
	case RFM12_QUEUE_RX:
		break;		// Receive
	default:
		goto receive;
	}
	uint8_t len = rfm12_rx_len();
	uint8_t type = rfm12_rx_type();
	uint8_t *buffer = rfm12_rx_buffer();
	memcpy(rxBuffer, buffer, len);
	rfm12_rx_clear();
	rxLength = len;
	rxType = type;
	packages.rx++;
	bytes.rx += len;
	goto loop;
}

void cntTask(void *param)
{
	TickType_t xLastWakeTime = 0;	// Same starting tick reference
	counter_t lPkgs = {0, 0}, lBytes = {0, 0};
loop:
	vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ * 5);
	counter_t nPkgs, nBytes;
	nPkgs.tx = packages.tx;
	nPkgs.rx = packages.rx;
	nBytes.tx = bytes.tx;
	nBytes.rx = bytes.rx;
	pPkgs.tx = nPkgs.tx - lPkgs.tx;
	pPkgs.rx = nPkgs.rx - lPkgs.rx;
	pBytes.tx = nBytes.tx - lBytes.tx;
	pBytes.rx = nBytes.rx - lBytes.rx;
	lPkgs = nPkgs;
	lBytes = nBytes;
	goto loop;
}

void tftTask(void *param)
{
	using namespace tft;
	using namespace colours::b16;
	uint16_t yy = y;
loop:
	x = 0;
	y = yy;
	setFont(10, 16);
	foreground = Orange;
	printf_P(PSTR("TX: %8lu / %8lu\n"), packages.tx, bytes.tx);
	printf_P(PSTR("5s: %8lu / %8lu\n"), pPkgs.tx, pBytes.tx);
	foreground = Yellow;
	printf_P(PSTR("RX: %8lu / %8lu\n"), packages.rx, bytes.rx);
	printf_P(PSTR("5s: %8lu / %8lu\n"), pPkgs.rx, pBytes.rx);
	if (packages.rx != 0) {
		foreground = Green;
		printf_P(PSTR("RX buffer (0x02x):\n"), rxType);
		setFont(6, 8);
		foreground = 0x667f;
		char *ptr = rxBuffer;
		for (uint8_t i = rxLength; i != 0; i--)
			putchar(*ptr++);
	}
	vTaskDelay(configTICK_RATE_HZ * 100 / 1000);
	goto loop;
}

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
	puts("Initialised.");
	tft::setBGLight(true);

	xTaskCreate(rfm12_int_task, "RFM12INT", configMINIMAL_STACK_SIZE, \
			NULL, tskIDLE_PRIORITY + 3, NULL);
	xTaskCreate(rfm12_tick_task, "RFM12TICK", configMINIMAL_STACK_SIZE, \
			NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(rfm12_layer, "LAY_RFM12", configMINIMAL_STACK_SIZE, \
			NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(tftTask, "TFT Task", configMINIMAL_STACK_SIZE * 3, \
			NULL, tskIDLE_PRIORITY, NULL);
	xTaskCreate(cntTask, "Counter", configMINIMAL_STACK_SIZE, \
			NULL, tskIDLE_PRIORITY, NULL);
	puts("Tasks created.");
	vTaskStartScheduler();

	return 1;
}
