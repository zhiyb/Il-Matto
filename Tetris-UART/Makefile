# Modified by zhiyb (Yubo Zhi)

TRG	= Tetris_UART
SRC	= ascii.c display.c ili9341.c main.c rgbconv.c sound.c tetris.c tft.c timer.c uart.c
SUBDIRS	= eemem

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

DEFS	+= -DBAUD=9600

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
