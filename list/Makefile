# Modified by zhiyb (Yubo Zhi)

TRG	= list
SRC	= main.cpp menu.cpp menufunc.cpp
SUBDIRS	= tft-cpp list rTouch common

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000
OPTLEVEL	= s

RTOUCH_SWAPXY	= n

#DEFS	+= -DDEBUG
LIBS	+= -lm

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
