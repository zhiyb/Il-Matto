# Modified by zhiyb (Yubo Zhi)

PRG	= rTouch
SRC	= main.cpp
LIB	= tft-cpp rTouch common
LIBDIR	= ../lib
VERSION	= 0.0.1

PRGER	= usbasp
MCU_TARGET	= atmega644p
OPTIMIZE	= -Os

# FAST define: no checks etc
DEFS	+= -DF_CPU=12000000
#DEFS	+= -DRTOUCH_SAFE
#DEFS	+= -DRTOUCH_SWAPXY
LIBS	+= -lm
INCS	+=

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

LOG	= ~/.make_avr.log

include ../Makefile.in
