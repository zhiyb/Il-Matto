# Modified by zhiyb (Yubo Zhi)

TRG	= TFT-CPP
SRC	= main.cpp
LIB	= tft-cpp capture
LIBDIR	= ../lib

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

#DEFS	+= -DTFT_VERTICALSCROLLING
DEFS	+= -DTFT_PORT_CTRL=A
DEFS	+= -DTFT_PORT_DATA=C
LIBS	+= -lm

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

include ../Makefile.defs
