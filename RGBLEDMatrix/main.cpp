#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <colours.h>

#define ROW_MASK	0x7
#define SIN		_BV(3)
#define DCK		_BV(4)
#define GCK		_BV(5)
#define SELBK		_BV(6)
#define LAT		_BV(7)

void init()
{
	// Port initialisation
	DDRD = 0xff;
	PORTD = 0xff;

	// UART1 in SPI mode
	// Transmit only
	UCSR1B = _BV(TXEN1);
	// Master SPI mode, MSB first, data setup at rising edge
	UCSR1C = _BV(UMSEL11) | _BV(UMSEL10);
	// Clock: fOSC / 2
	UBRR1 = 0;

	// Timer1.A PWM
	TCCR1A = _BV(COM1A1) | _BV(WGM11);
	TCCR1B = _BV(WGM13) | _BV(WGM12);
	TCCR1C = 0;
	OCR1A = 0;
	ICR1 = 1;
	TCCR1B |= _BV(CS10);
}

int main()
{
	init();

	uint8_t row = 0;
	uint8_t data = 0;
	uint16_t i;
	PORTD &= ~SELBK;
	for (i = 0; i != 6 * 3 * 4 * 8 / 8; i++) {
		while (!(UCSR1A & _BV(UDRE1)));
		UDR1 = 0xff;
		//UCSR1A = _BV(UDRE1);
	}
	while (!(UCSR1A & _BV(TXC1)));
	UCSR1A = _BV(TXC1);
	PORTD |= LAT;
	PORTD |= LAT;
	PORTD |= LAT;
	PORTD &= ~LAT;

	PORTD |= SELBK;
loop:
	uint8_t col;
	uint32_t clr = colour_hsv_to_rgb(COLOUR_888(data, 255, 127));
	for (col = 0; col != 8 * 4; col++) {
		while (!(UCSR1A & _BV(UDRE1)));
		UDR1 = BLUE_888(clr);
		while (!(UCSR1A & _BV(UDRE1)));
		UDR1 = GREEN_888(clr);
		while (!(UCSR1A & _BV(UDRE1)));
		UDR1 = RED_888(clr);
	}
	while (!(UCSR1A & _BV(TXC1)));
	UCSR1A = _BV(TXC1);
	PORTD |= LAT;
	PORTD |= LAT;
	PORTD |= LAT;
	PORTD = (PORTD & ~ROW_MASK) | (row & ROW_MASK);
	PORTD &= ~LAT;

	row++;
	if (row == 0)
		data++;
	goto loop;
	return 0;
}
