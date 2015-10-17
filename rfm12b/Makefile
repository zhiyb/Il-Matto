# Modified by zhiyb (Yubo Zhi)

TRG	= rfm12b
SRC	= main.cpp
SUBDIRS	= rfm12b tft-cpp common

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
