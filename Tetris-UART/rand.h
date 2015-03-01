#ifndef _RAND_H
#define _RAND_H

#include <avr/eeprom.h>

extern uint32_t EEMEM NVrandSeed;

void Rand_init(void);

#endif
