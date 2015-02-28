#ifndef EEPROM_H
#define EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/eeprom.h>

#define EE_INIT_SPEC	0xAA			// First run sign

uint8_t eeprom_first(void);
void eeprom_first_done(void);

#ifdef __cplusplus
}
#endif

#endif
