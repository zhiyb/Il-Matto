# Modified by zhiyb (Yubo Zhi)

PRG	= list
SRC	= main.cpp menu.cpp
LIB	= tft-cpp list rTouch common
LIBDIR	= ../lib
VERSION	= 0.0.1

PRGER	= usbasp
MCU_TARGET	= atmega644p
OPTIMIZE	= -Os

DEFS	+= -DF_CPU=12000000 -DDEBUG
DEFS	+= -DTFT_VERTICALSCROLLING
DEFS	+= -DRTOUCH_SWAPXY
LIBS	+=
INCS	+=

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

LOG	= ~/.make_avr.log

include ../Makefile.in
