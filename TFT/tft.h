#ifndef _TFT_H
#define _TFT_H

#include <stdio.h>
#include "ili9341.h"

class tfthw : public ili9341
{
public:
	tfthw(void);

	class tfthw& operator<<(const char c);
	class tfthw& operator<<(const char *str);
	class tfthw& operator<<(const int16_t i);
	class tfthw& operator<<(const uint16_t i);
	class tfthw& operator<<(const int32_t i);
	class tfthw& operator<<(const uint32_t i);
	class tfthw& operator++(int x);
	class tfthw& operator--(int x);
	class tfthw& operator*=(uint8_t z);
	class tfthw& operator/=(uint8_t o);

	virtual void setX(uint16_t n) {x = n;}
	virtual void setY(uint16_t n) {y = n;}
	virtual void setXY(uint16_t m, uint16_t n) {x = m; y = n;}
	virtual uint16_t getX(void) {return x;}
	virtual uint16_t getY(void) {return y;}
	virtual void setForeground(uint16_t c) {fc = c;}
	virtual void setBackground(uint16_t c) {bc = c;}
	virtual uint8_t getForeground(void) {return fc;}
	virtual uint8_t getBackground(void) {return bc;}
	virtual void setOrient(uint8_t o);
	virtual uint8_t getOrient(void) {return orient;}
	virtual void setBGLight(bool e);
	virtual void clean(void) {fill(bc); x = 0; y = 0;}
	virtual void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, \
		uint16_t c);
	virtual void frame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint8_t s, uint16_t c);
	virtual void fill(uint16_t clr);
	virtual void putch(uint16_t x, uint16_t y, char ch, \
		uint16_t fgc, uint16_t bgc);
	virtual void putbigch(uint16_t x, uint16_t y, uint8_t z, char ch, \
		uint16_t fgc, uint16_t bgc);
	virtual void putint32(uint16_t x, uint16_t y, int32_t i, \
		uint16_t fgc, uint16_t bgc);
	virtual void putbigint32(uint16_t x, uint16_t y, uint8_t z, int32_t i, \
		uint16_t fgc, uint16_t bgc);
	virtual void puts(uint16_t x, uint16_t y, char *str, \
		uint16_t fgc, uint16_t bgc);
	virtual void putbigs(uint16_t x, uint16_t y, uint8_t z, char *str, \
		uint16_t fgc, uint16_t bgc);
	virtual void rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint16_t c);
	virtual void point(uint16_t x, uint16_t y, uint16_t c);

protected:
	virtual void area(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
	virtual void all();
	virtual void newline(void);
	virtual void next(void);

	uint8_t zoom, orient;
	uint16_t x, y, w, h, fc, bc;
};

FILE *tftout(void);

extern class tfthw tft;

#endif
