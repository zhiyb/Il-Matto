#include <avr/eeprom.h>
#include <stdlib.h>
#include <eemem.h>

uint32_t EEMEM NVrandSeed;

void Rand_init(void)
{
	if (eepromFirst()) {
		srand(0);
		eeprom_update_dword(&NVrandSeed, 1);
		return;
	}
	srand(eeprom_read_dword(&NVrandSeed));
	eeprom_update_dword(&NVrandSeed, eeprom_read_dword(&NVrandSeed) + 1);
}
