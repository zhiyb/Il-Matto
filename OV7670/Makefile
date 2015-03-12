# Modified by zhiyb (Yubo Zhi)

PRG	= OV7670
SRC	= main.cpp
LIB	= tft-cpp OV7670-cpp_AC
LIBDIR	= ../lib
VERSION	= 0.0.1

PRGER	= usbasp
MCU_TARGET	= atmega644p
OPTIMIZE	= -Os

DEFS	+= -DF_CPU=12000000
#DEFS	+= -DTFT_CHECK
#DEFS	+= -DTFT_VERTICALSCROLLING
LIBS	+=
INCS	+=

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

LOG	= ~/.make_avr.log

include ../Makefile.inc
