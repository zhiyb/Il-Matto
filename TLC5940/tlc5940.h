#ifndef TLC5940_H
#define TLC5940_H

#define TLC5940_PORT	D
#define TLC5940_XLAT	_BV(0)
#define TLC5940_DCPRG	_BV(1)
#define TLC5940_XERR	_BV(2)	// RXD1
#define TLC5940_SIN	_BV(3)	// TXD1
#define TLC5940_SCLK	_BV(4)	// XCK1
#define TLC5940_BLANK	_BV(5)	// OC1A
#define TLC5940_GSCLK	_BV(6)	// OC2B
#define TLC5940_VPRG	_BV(7)	// OC2A

void tlc5940_init();
//void tlc5940_set_dc();
// Set grayscale data, data is 8 bits for offset == 1, 12 bits for offset > 1
void tlc5940_set_gs(uint8_t count, uint8_t offset, const void *ptr);
void tlc5940_blank(const uint8_t blank);

#endif
