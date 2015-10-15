/*
 * Author: Yubo Zhi (normanzyb@gmail.com)
 */

#include <inttypes.h>
#include <avr/eeprom.h>
#include "eemem.h"

#define EEMEM_INIT_PATTERN	0xAA	// First time recognition pattern

static uint8_t EEMEM NVfirst = (uint8_t)~EEMEM_INIT_PATTERN;
static uint8_t first = EEMEM_INIT_PATTERN;
static uint32_t EEMEM NVcount = 0;

uint8_t eeprom_first()
{
	if (first == EEMEM_INIT_PATTERN)
		first = eeprom_read_byte(&NVfirst) != EEMEM_INIT_PATTERN;
	return first;
}

void eeprom_done()
{
	if (eeprom_first())
		eeprom_update_byte(&NVfirst, EEMEM_INIT_PATTERN);
}

uint32_t eeprom_counter()
{
	return eeprom_read_dword(&NVcount);
}

void eeprom_counter_increment()
{
	if (eeprom_first())
		eeprom_update_dword(&NVcount, 1);
	else
		eeprom_update_dword(&NVcount, eeprom_counter() + 1);
}
