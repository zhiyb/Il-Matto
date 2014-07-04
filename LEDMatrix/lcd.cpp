#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"
#include "display.h"

#define CMD_CLEAR()		(0x01)
#define CMD_RETURN()		(0x02)
#define CMD_ENTRY(id, s)	(0x04 | id << 1 | s)
// Args: D: All display, C: Cursor, B: Cursor position
#define CMD_STATUS(d, c, b)	(0x08 | d << 2 | c << 1 | b)
#define CMD_CURSOR(sc, rl)	(0x10 | sc << 3 | rl << 2)
// Args: RE: Extend(1)/Basic(0)
#define CMD_FUNCTION(re)	(0x30 | re << 2)
#define CMD_CGRAM(addr)		(0x40 | addr)
#define CMD_DDRAM(addr)		(0x80 | addr)

namespace lcd
{
	enum Types {Data = 0, Cmd = 1};
	void wait(void);
	void write(bool cmd, uint8_t data);
}

using namespace display;

void lcd::wait(void)
{
	LCD_DDDR = 0x00;
	LCD_DPORT = 0xFF;
	LCD_CPORT &= ~LCD_RS;
	LCD_CPORT |= LCD_RW;
	LCD_CPORT |= LCD_E;
	while (LCD_DPIN & 0x80);
	LCD_CPORT &= ~LCD_E;
	LCD_DDDR = 0xFF;
}

void lcd::write(bool cmd, uint8_t data)
{
	wait();
	LCD_DPORT = data;
	if (cmd)
		LCD_CPORT &= ~LCD_RS;
	else
		LCD_CPORT |= LCD_RS;
	LCD_CPORT &= ~LCD_RW;
	LCD_CPORT |= LCD_E;
	LCD_CPORT &= ~LCD_E;
}

void lcd::init(void)
{
	LCD_DPORT = 0;
	LCD_CPORT &= ~(LCD_RS | LCD_RW | LCD_E | LCD_RST);
	LCD_DDDR = 0xFF;
	LCD_CDDR |= LCD_RS | LCD_RW | LCD_E | LCD_RST;
	_delay_ms(40);
	LCD_CPORT |= LCD_RST;

	write(Cmd, CMD_FUNCTION(0));
	write(Cmd, CMD_FUNCTION(0));
	write(Cmd, CMD_CLEAR());
	wait();
	write(Cmd, CMD_RETURN());
	wait();
	write(Cmd, CMD_ENTRY(1, 0));
	write(Cmd, CMD_DDRAM(0));
	write(Data, 'T');
	write(Data, 'e');
	write(Data, 's');
	write(Data, 't');
	write(Cmd, CMD_STATUS(1, 0, 0));
}

void lcd::update(void)
{
}
