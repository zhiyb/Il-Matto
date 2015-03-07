#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "dac.h"

// In <avr/cpufunc.h>, but WinAVR doesn't have the file
#define _NOP() __asm__ __volatile__("nop")

static volatile uint8_t dac_data, dac_tx_status;

void init_dac(void)
{
	dac_tx_status = 0;

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
	dac_data = data;
	// Enable data register empty interrupt
	UCSR1B |= _BV(UDRE1);
	return;
}

// USART1 Data register empty interrupt
ISR(USART1_UDRE_vect)
{
	UDR1 = dac_data;
	// Disable data register empty interrupt
	UCSR1B &= ~_BV(UDRE1);
	// Enable transmit complete interrupt
	UCSR1B |= _BV(TXC1);
}

// USART1 Transmit complete interrupt
ISR(USART1_TX_vect, ISR_NOBLOCK)
{
	if (!dac_tx_status) {	// Transmitted channel configure byte
		dac_tx_status++;
		return;
	}
	PORTD &= ~DAC_LOAD;	// Lowing DAC_LOAD to load
	_NOP();		// tW(LOAD) min. 250ns
	dac_tx_status = 0;
	// Disable transmit complete interrupt
	UCSR1B &= ~_BV(TXC1);
	PORTD |= DAC_LOAD;
}
