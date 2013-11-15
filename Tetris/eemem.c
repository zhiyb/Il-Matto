#include <inttypes.h>
#include <avr/eeprom.h>
#include "eemem.h"

#define SPEC 0x55			// First run sign
uint8_t first = 5;

uint8_t EEPROM_first(void)
{
	if (first == 5)
		first = eeprom_read_byte(EE_FIRST_SIGN) != SPEC;
	if (first)
		eeprom_write_byte(EE_FIRST_SIGN, SPEC);
	return first;
}
