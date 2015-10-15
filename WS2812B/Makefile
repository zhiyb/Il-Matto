# Author: Yubo Zhi (normanzyb@gmail.com)

TRG	= WS2812B
SRC	= main.cpp
SUBDIRS	= common rgbled FreeRTOS tft-cpp
INCDIRS	= .

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

RTOSPORT	= ATMega644P

RGBLED_PORT	= B
RGBLED_DIN	= 7
RGBLED_NUM	= 8

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
