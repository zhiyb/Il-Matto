/*
 * Author: Yubo Zhi (normanzyb@gmail.com)
 */

#ifndef EEMEM_H
#define EEMEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Check if EEPROM uninitialised (first time boot up)
uint8_t eeprom_first();
// Done EEPROM first time initialisation
void eeprom_done();
// Returns (startup) counter
uint32_t eeprom_counter();
// Increment (startup) counter
void eeprom_counter_increment();

#ifdef __cplusplus
}
#endif

#endif
