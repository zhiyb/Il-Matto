#ifndef DISPLAY_H
#define DISPLAY_H

#define CHECK_DDR	DDRC
#define CHECK_PORT	PORTC
#define CHECK_PIN	PINC
#define CHECK_BIT	_BV(PC4)

#define LED_W	64
#define LED_H	32

#define LCD_W	128
#define LCD_H	64

#define BUFF_W	LED_W
#define BUFF_H	LED_H

#include <inttypes.h>
#include <stdio.h>

typedef int8_t int_t;
typedef uint8_t uint_t;

namespace display
{
	enum Levels {Foreground = 0, Background = 2};
	enum Colours {None = 0, Red = 1, Green = 2, Orange = Red | Green, Yellow = Orange, \
		AllRed = Red << Foreground | Red << Background, AllGreen = Green << Foreground | Green << Background, \
		FGC = ~(0xFF << Background), BGC = ~FGC};
	enum BuffColours {BuffRed = 0, BuffGreen = 1};

	// Row, Column, Colour
	extern volatile uint_t buff[BUFF_H][BUFF_W / 8][2];
	extern bool lcdOutput;
}

class Display
{
public:
	Display(void);

	static void init(void);
	static void update(void);
	static FILE *out(void);

	void setX(uint_t _x) {x = _x;}
	void setY(uint_t _y) {y = _y;}
	void setXY(uint_t _x, uint_t y) {setX(x); setY(y);}
	void setColour(uint_t colour) {clr = colour;}

	void fill(const uint_t clr);
	void clear(void) {fill(clr & display::BGC); setXY(0, 0);}
	void drawPoint(const uint_t x, const uint_t y, const uint_t clr);
	void drawChar(const uint_t x, const uint_t y, const uint_t zoom, const char ch);
	void drawChar(const char ch);
	void drawString(const uint_t x, const uint_t y, const uint_t zoom, const char *str);
	void drawEllipse(uint_t xx, uint_t yy, int_t w, int_t h);
	void drawLine(uint_t x0, uint_t y0, uint_t x1, uint_t y1);

	Display& operator<<(const char c);

private:
	void newline(void);
	void next(void);
	void tab(void);

	uint8_t zoom, tabSize;
	uint_t x, y, clr;
};

extern Display disp;

#endif
