/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#include <inttypes.h>
#include <avr/eeprom.h>
#include "eemem.h"

#ifndef EEMEM_INIT_PATTERN
#define EEMEM_INIT_PATTERN	0xAA	// First time recognition pattern
#endif

static uint8_t EEMEM NVfirst = (uint8_t)~EEMEM_INIT_PATTERN;
static uint8_t first = EEMEM_INIT_PATTERN;

uint8_t eepromFirst(void)
{
	if (first == EEMEM_INIT_PATTERN)
		first = eeprom_read_byte(&NVfirst) != EEMEM_INIT_PATTERN;
	return first;
}

void eepromFirstDone(void)
{
	if (first)
		eeprom_update_byte(&NVfirst, EEMEM_INIT_PATTERN);
}
