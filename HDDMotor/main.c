#include <avr/io.h>
#include <util/delay.h>

#define USE_5V

#ifdef USE_5V
#define RINIT	150
#define RMIN	1
#define RTH	30
#define RFACT	1

#define DINIT	10
#define DMIN	4
#define DFACT	1
#define D2	1

#define	BREAK	40
#define STEP	2
#else
#define RINIT	254
#define RMIN	1
#define RTH	14
#define RFACT	1

#define DINIT	20
#define DMIN	10
#define DFACT	1
#define D2	1

#define	BREAK	10
#define STEP	2
#endif

#define PINS	(_BV(5) | _BV(6) | _BV(7))

#if 1
const uint8_t seq[6][2] = {
	{_BV(7),		_BV(7)},
	{_BV(6) | _BV(7),	_BV(6) | _BV(7)},
	{_BV(6),		_BV(6)},
	{_BV(5) | _BV(6),	_BV(5) | _BV(6)},
	{_BV(5),		_BV(5)},
	{_BV(5) | _BV(7),	_BV(5) | _BV(7)},
};
#else
const uint8_t seq[6][2] = {
	{_BV(5),		_BV(5)},
	{_BV(5) | _BV(6),	_BV(5) | _BV(6)},
	{_BV(6),		_BV(6)},
	{_BV(6) | _BV(7),	_BV(6) | _BV(7)},
	{_BV(7),		_BV(7)},
	{_BV(5) | _BV(7),	_BV(5) | _BV(7)},
};
#endif

uint8_t r = RINIT, d = DINIT;

void delay(uint16_t d)
{
	while (d--)
		_delay_us(10);
}

void faster(void)
{
	if (r > RTH || d == DMIN) {
		if (r > RMIN + RFACT)
			r -= RFACT;
		else
			r = RMIN;
	} else if (r <= RTH) {
		if (d > DMIN + DFACT)
			d -= DFACT;
		else
			d = DMIN;
	}
}

void slower(void)
{
	if (r < RINIT + RFACT)
		r += RFACT;
	else
		r = RINIT;
	if (r > RTH) {
		if (d < DINIT + DFACT)
			d += DFACT;
		else
			d = DINIT;
	}
}

void init(void)
{
	MCUCR |= 0x80;					// Disable JTAG
	MCUCR |= 0x80;

	DDRB |= _BV(7);
	PORTB = 0;

	DDRD |= PINS;
	PORTD = 0x00;
	DDRC &= ~PINS;
	PORTC |= PINS;

	DDRB |= _BV(4);
	TCCR0B = 0;
	TCCR0A = _BV(COM0B1) | _BV(WGM00) | _BV(WGM01);	// Fast PWM
	OCR0B = 128;
	TIMSK0 = 0;
	TCCR0B = _BV(CS00);				// Start PWM
}

int main(void)
{
	init();

#if 0
	while (1) {
		uint8_t i;
		for (i = 0; i < 6; i++) {
			do {
				PORTD = seq[i][0];
				_delay_us(2000);
				PORTD = 0;
				_delay_us(2000);
			} while (0);
		}
	}
#else
	while (1) {
		uint8_t i;
		for (i = 0; i < 6; i++) {
			uint8_t j = 0;
			do {
				PORTD = seq[i][0];
				delay(r);
				PORTD = 0;
				delay(d);
				if (j == 0 && (PINC & PINS) == seq[i][1]) {
					faster();
					break;
				}
				delay(D2);
				if ((PINC & PINS) == seq[i][1])
					break;
				if (j > STEP)
					slower();
				if (j > BREAK)
					break;
				j++;
			} while ((PINC & PINS) != seq[i][1]);
		}
	}
#endif

	return 1;
}
