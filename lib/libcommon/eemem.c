/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#include <inttypes.h>
#include <avr/eeprom.h>
#include "eemem.h"

static uint8_t EEMEM NVfirst;
static uint8_t first = EE_INIT_SPEC;

uint8_t eeprom_first(void)
{
	if (first == EE_INIT_SPEC)
		first = eeprom_read_byte(&NVfirst) != EE_INIT_SPEC;
	return first;
}

void eeprom_first_done(void)
{
	if (first)
		eeprom_update_byte(&NVfirst, EE_INIT_SPEC);
}
