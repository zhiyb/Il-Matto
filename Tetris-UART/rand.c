#include <avr/eeprom.h>
#include <stdlib.h>
#include "eemem.h"

void Rand_init(void)
{
	if (EEPROM_first()) {
		srand(0);
		eeprom_update_dword(EE_RAND_SEED, 1);
		return;
	}
	srand(eeprom_read_dword(EE_RAND_SEED));
	eeprom_update_dword(EE_RAND_SEED, eeprom_read_dword(EE_RAND_SEED) + 1);
}
