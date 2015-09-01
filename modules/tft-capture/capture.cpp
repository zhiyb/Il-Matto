#ifdef TFT_READ_AVAILABLE

#include <avr/interrupt.h>
#include <avr/io.h>
#include <tft.h>
#include "capture.h"

#define BAUD	CAPTURE_BAUD

namespace capture
{
	void send(void);
	void write(uint8_t data);
}

void capture::init()
{
	// Initialise UART1
	#include <util/setbaud.h>
	DDRD &= ~0x02;
	DDRD |= 0x03;
	PORTD |= 0x02 | 0x03;
	UBRR1H = UBRRH_VALUE;
	UBRR1L = UBRRL_VALUE;
	UCSR1A = USE_2X << U2X1;
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	UCSR1C = (1 << UCSZ10) | (1 << UCSZ11);

	// Clear flags
	UCSR1A |= _BV(TXC1);
	UDR1;
	
	disable();
}

void capture::enable(void)
{
	// Clear flags
	UCSR1A |= _BV(TXC1);
	UDR1;
	// Interrupt
	UCSR1B |= _BV(RXCIE1);
}

void capture::disable(void)
{
	UCSR1B &= ~_BV(RXCIE1);
}

void capture::write(uint8_t data)
{
	while (!(UCSR1A & _BV(UDRE1)));
	UDR1 = data;
}

void capture::send(void)
{
	tfthw::all();
	tfthw::memRead();	// Read
	tfthw::mode(true);	// Read mode
	tfthw::read();
	for (uint16_t y = 0; y < tft::height; y++)
		for (uint16_t x = 0; x < tft::width; x++) {
			write(tfthw::read());
			write(tfthw::read());
			write(tfthw::read());
		}
	tfthw::mode(false);	// Write mode
}

ISR(USART1_RX_vect)
{
	if (UDR1 != 'W')
		return;
	capture::disable();
	capture::send();
	capture::enable();
}

#endif	// TFT_READ_AVAILABLE
