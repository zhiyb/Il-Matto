# Modified by zhiyb (Yubo Zhi)

TRG	= rTouch
SRC	= main.cpp
SUBDIRS	= tft-cpp rTouch common tft-capture misc eemem request

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

# FAST: no checks etc
RTOUCH_SAFE	= n
RTOUCH_SWAPXY	= n

LIBS	+= -lm

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
