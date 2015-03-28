# Modified by zhiyb (Yubo Zhi)

TRG	= rTouch
SRC	= main.cpp
LIB	= tft-cpp rTouch common capture
LIBDIR	= ../lib

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

# FAST define: no checks etc
DEFS	+= -DF_CPU=12000000
#DEFS	+= -DRTOUCH_SAFE
#DEFS	+= -DRTOUCH_SWAPXY
LIBS	+= -lm
INCS	+=

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

include ../Makefile.defs
