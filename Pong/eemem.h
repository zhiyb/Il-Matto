#ifndef _EEPROM_H
#define _EEPROM_H

#define EE_FIRST_SIGN (uint8_t *)(1024)		// First run sign
#define EE_PONG_SCORE0 (uint16_t *)(0)		// Pong: highest score 0
#define EE_PONG_SCORE1 (uint16_t *)(2)		// Pong: highest score 1
#define EE_PONG_OVERREC0 (uint16_t *)(4)	// Pong: game over count
#define EE_PONG_OVERREC1 (uint16_t *)(6)	// Pong: game over count

uint8_t EEPROM_first(void);

#endif
