# Modified by zhiyb (Yubo Zhi)

TRG	= TLC5940
SRC	= main.c tlc5940.c
SUBDIRS	= common
INCDIRS	=

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

LIBS	+= -lm

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
