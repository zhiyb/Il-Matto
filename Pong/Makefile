# Modified by zhiyb (Yubo Zhi)

TRG	= Pong
SRC	= game.c main.c rotary.c sound.c
SUBDIRS	= tft eemem

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000
OPTLEVEL	= s

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
