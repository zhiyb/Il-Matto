# Modified by zhiyb (Yubo Zhi)

TRG	= 2048
SRC	= main.cpp 2048.cpp
SUBDIRS	= rTouch tft-cpp common misc request eemem

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

TFT_HARDWARE	= ili9341_parallel
TFT_PORT_CTRL	= A
TFT_PORT_DATA	= C
TFT_FONTS	= y

LIBS	+= -lm

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
