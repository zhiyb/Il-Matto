#ifndef _EEPROM_H
#define _EEPROM_H

#define EE_RAND_SEED (uint32_t *)(2048 - 4)	// Random generator seed
#define EE_FIRST_SIGN (uint8_t *)(2048 - 5)	// First run sign
#define EE_TETRIS_HIGH (uint16_t *)(0)		// Tetris: highest score
#define EE_TETRIS_SAVE (uint16_t *)(2)		// Tetris: save

uint8_t EEPROM_first(void);

#endif
