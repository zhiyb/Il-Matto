# Modified by zhiyb (Yubo Zhi)

TRG	= list
SRC	= main.cpp menu.cpp menufunc.cpp
LIB	= tft-cpp list rTouch common
LIBDIR	= ../lib
VERSION	= 0.0.1

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000
OPTLEVEL	= s

#DEFS	+= -DDEBUG
DEFS	+= -DTFT_VERTICALSCROLLING
DEFS	+= -DRTOUCH_SWAPXY
LIBS	+= -lm
INCS	+=

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

include ../Makefile.defs
