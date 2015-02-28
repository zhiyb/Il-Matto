#include <avr/io.h>
#include <util/delay.h>
#include "dac.h"

void init_dac(void)
{
	// Port initialisation
	DDRD |= DAC_LOAD | DAC_DATA | DAC_CLK;
	PORTD |= DAC_LOAD | DAC_DATA | DAC_CLK;
	// Configure UART1 in SPI mode
	// Enable transmit only
	UCSR1B = _BV(TXEN1);
	// Master SPI mode, MSB first, data setup at rising edge
	UCSR1C = _BV(UMSEL11) | _BV(UMSEL10) | _BV(UCPHA1);
	// BAUD = FOSC / (2 * (UBRR1 + 1))
	UBRR1H = 0;
	// Clock frequency max. 1MHz
	UBRR1L = F_CPU / 1000000 / 2 - 1;
	// Clear transmit complete flag
	UCSR1A |= _BV(TXC1);
}


void set_dac(uint8_t data)
{
	UDR1 = DAC_CHANNEL * 2 + 1;	// RNG = 1 for gain of 2x from ref
	while (!(UCSR1A & _BV(UDRE1)));
	UDR1 = data;
	while (!(UCSR1A & _BV(TXC1)));	// Need to wait for transmit complete
	UCSR1A |= _BV(TXC1);
	PORTD &= ~DAC_LOAD;		// Lowing DAC_LOAD to load
	_delay_us(1);			// tW(LOAD) > 250ns
	PORTD |= DAC_LOAD;
}
