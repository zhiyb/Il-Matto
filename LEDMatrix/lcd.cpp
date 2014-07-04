#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"
#include "display.h"

// Basic function set
#define CMD_CLEAR()		(0x01)
#define CMD_HOME()		(0x02)
#define CMD_ENTRY(id, s)	(0x04 | id << 1 | s)
// Args: D: All display, C: Cursor, B: Cursor position
#define CMD_DISPLAY(d, c, b)	(0x08 | d << 2 | c << 1 | b)
#define CMD_CURSOR(sc, rl)	(0x10 | sc << 3 | rl << 2)
// Args: DL: 8-bit(1)/4-bit(0), RE: Extend(1)/Basic(0), G: Graphic display
#define CMD_FUNCTION(dl, re, g)	(0x20 | dl << 4 | re << 2 | g << 1)
#define CMD_CGRAM(addr)		(0x40 | addr)
#define CMD_DDRAM(addr)		(0x80 | addr)

// Extended fucntion set
#define CMD_STANDBY()		(0x01)
// Args: SR=1: vertical scroll position, SR=0: CGRAM address
#define CMD_SELECT(sr)		(0x02 | sr)
// Args: R: row select (0-3)
#define CMD_REVERSE(r)		(0x04 | r)
#define CMD_SCROLL(addr)	(0x40 | addr)
#define CMD_GDRAM(addr)		(0x80 | addr)

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

	write(Cmd, CMD_CLEAR());
	write(Cmd, CMD_FUNCTION(1, 0, 1));
	write(Cmd, CMD_FUNCTION(1, 1, 1));
	write(Cmd, CMD_SELECT(1));
	write(Cmd, CMD_SCROLL(0));
	for (uint8_t r = 0; r < 32; r++) {
		write(Cmd, CMD_GDRAM(r));
		write(Cmd, CMD_GDRAM(0x00));
		for (uint8_t c = 0; c < 32; c++)
			write(Data, 0x00);
	}
}

void lcd::update(void)
{
	for (uint8_t r = 0; r < 32; r++) {
		write(Cmd, CMD_GDRAM(r));
		write(Cmd, CMD_GDRAM(0x00));
		for (uint8_t d = 0; d < 2; d++)	// Up(0)/Lower(1) half
			for (uint8_t c = 0; c < 32 / 2 / 2; c++) {
				uint8_t data = buff[r / 2 + d * 16] \
					       [c][BuffRed];
				data |= buff[r / 2 + d * 16][c][BuffGreen];
				uint8_t l = 0, r = 0;
				for (uint8_t i = 0; i < 8; i++) {
					uint8_t move = i / 2;
					l |= ((data >> move) & 0x01) \
					     << (7 - i);
					r |= ((data << move) & 0x80) \
					     >> (7 - i);
				}
				write(Data, l);
				write(Data, r);
			}
	}
}
