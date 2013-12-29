#include <avr/interrupt.h>
#include "sound.h"
#include "timer.h"

volatile uint8_t count = 0;

ISR(TIMER1_COMPA_vect)
{
	SOUND_PIN |= SOUND_P;	// Toggle pin
	if (count++ != SOUND_LENGTH)
		return;
	count = 0;
	timer1_disable();
	SOUND_PORT &= ~SOUND_P;
}
