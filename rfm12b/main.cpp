// Author: Yubo Zhi (normanzyb@gmail.com)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
//#include <tft.h>
//#include <colours.h>
#include <rfm12_config.h>
#include <rfm12.h>
#include <uart0.h>
#include "mac_layer.h"
#include "tcp_layer.h"
#include "sampler.h"
#include "pwm2.h"

#define SMP_SIZE	(128 + 64)

//FILE *tftout;

void init()
{
#if 0
	tft::init();
	tftout = tft::devout();
#endif
#if 1
	uart0_init();
	stdout = uart0_fd();
	stdin = uart0_fd();
#endif
	_delay_ms(1000);
	rfm12_init();
	_delay_ms(1000);
	sei();

	sampler_init();
	pwm2_init();
	sampler_enable(1);
	pwm2_enable(1);
}

int main()
{
	init();
	puts("Initialised.");
	//fputs("Initialised.\n", tftout);
	//tft::setBGLight(true);

	// Sampler
	//static uint8_t smpBuffer[128];
	static uint8_t *smpBuffer;
	smpBuffer = payloadBuffer();
	static uint8_t *sptr;
        sptr = smpBuffer;
	static bool smpReady = false;

	// PWM, double buffering
	static uint8_t pwmBuffer[2][SMP_SIZE];
	// Current reading buffer
	static uint8_t pwmRead = 0;
	// Reading position (initially at the end)
	static uint8_t *pptr = pwmBuffer[0] + SMP_SIZE;
	static bool pwmReady = false;
	
	// UART
	static char buffer[200] = "\e[96mGroup 7, yz39g13 with jm15g13: \e[0m";
	const uint8_t len = strlen(buffer);
	char *ptr = buffer + len;
	static bool uartReady = false;

	// Miscellaneous
	uint16_t txCount = 0, rxCount = 0;
	int c;
	DDRC |= _BV(7);
	DDRC &= ~_BV(0);
	PORTC |= _BV(0);
poll:
	// If sampler is ready
	if ((PINC & _BV(0)) && sampler_available()) {
		if (smpReady)
			sampler_get();	// Reset sampler
		else {
			*sptr++ = sampler_get();
			if (sptr == smpBuffer + SMP_SIZE) {
				smpReady = true;
				sptr = smpBuffer;
			}
		}
	}

	// If data from sampler is ready for transmit
	if (smpReady)
		smpReady = send(MACHINE_ADDRESS, SMP_SIZE) != RFM12_TX_ENQUEUED;

	// If PWM buffer is filled
	if (sampler_tick()) {	// Same sample rate
		if (pwmReady) {
			// Another PWM buffer is filled
			PINC |= _BV(7);
			pwmReady = false;
			pwmRead = !pwmRead;
			pptr = pwmBuffer[pwmRead];
			pwm2_set(*pptr++);
		} else if (pptr != pwmBuffer[pwmRead] + SMP_SIZE) {
			// Current PWM buffer not fully read
			pwm2_set(*pptr++);
		}
	}

	// If something is received through UART
	if ((c = uart0_read_unblocked()) != -1) {
		if (!uartReady) {
			putchar(c);
			*ptr++ = c;
			if (c == '\n' || c == '\r') {
				*ptr++ = '\n';
				*ptr = 0;
				uartReady = true;
				ptr = buffer + len;
			}
		}
	}

	// If data from UART is ready for transmit
	if (uartReady) {
		uartReady = write(MACHINE_ADDRESS, strlen(buffer) + 1, (uint8_t *)buffer) != RFM12_TX_ENQUEUED;
		if (uartReady) {
			printf("Sent (0x%02x): %u\n", MACHINE_ADDRESS, txCount);
			//fprintf(tftout, "Sent: %u\n", txCount);
			txCount++;
			puts(buffer);
			//fputs(buffer, tftout);
			//fputc('\n', tftout);
		}
	}

	// If something is received through rfm12b
	uint8_t rxLen;
	if ((rxLen = available()) != 0) {
		uint8_t type = rfm12_rx_type();
		uint8_t src;
		if (rxLen == SMP_SIZE) {	// Sampler package
			read(&src, pwmBuffer[!pwmRead]);
			pwmReady = true;
			goto tick;
		}

		// General message
		static uint8_t buffer[RFM12_RX_BUFFER_SIZE];
		read(&src, buffer);
		printf_P(PSTR("\e[91mReceived %u: "), rxCount);
		//fprintf_P(tftout, PSTR("Received %u: "), rxCount++);
		rxCount++;
		printf_P(PSTR("length %u, type 0x%02x, from 0x%02x\n\e[92m"), rxLen, type, src);
		//fprintf_P(tftout, PSTR("length %u, type 0x%02x\n"), len, type);
		*(buffer + rxLen) = '\0';
		puts((char *)buffer);
		//fputs((char *)buffer, tftout);
		//fputc('\n', tftout);
		//rfm12_rx_clear();
	}

tick:
	//rfm12_tick();
	mac_tick(MAC_CSMA);
	_delay_us(5);
	goto poll;

	return 1;
}
