# Modified by zhiyb (Yubo Zhi)

TRG	= OV7670
SRC	= main.cpp
LIB	= tft-cpp OV7670-cpp_AC
LIBDIR	= ../lib

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

#DEFS	+= -DTFT_CHECK
#DEFS	+= -DTFT_VERTICALSCROLLING
LIBS	+=
INCS	+=

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

include ../Makefile.defs
