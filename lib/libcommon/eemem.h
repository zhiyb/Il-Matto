/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef EEMEM_H
#define EEMEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/eeprom.h>

uint8_t eepromFirst(void);
void eepromFirstDone(void);

#ifdef __cplusplus
}
#endif

#endif
