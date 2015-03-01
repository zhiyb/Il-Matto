# Modified by zhiyb (Yubo Zhi)

PRG	= Tetris
SRC	= ascii.c display.c eemem.c ili9341.c main.c rand.c rgbconv.c rotary.c sound.c tetris.c tft.c timer.c
LIB	= 
LIBDIR	= ../lib
VERSION	= 0.0.1

PRGER	= usbasp
MCU_TARGET	= atmega644p
OPTIMIZE	= -Os

DEFS	+= -DF_CPU=12000000 -DBAUD=9600
LIBS	+=
INCS	+=

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

LOG	= ~/.make_avr.log

include ../Makefile.in
