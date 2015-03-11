# Modified by zhiyb (Yubo Zhi)

PRG	:= TFT-CPP
SRC	:= main.cpp
SUBDIRS	:= capture
LIB	:= tft-cpp
LIBDIR	:= ../lib
VERSION	:= 0.0.1

PRGER	= usbasp
MCU_TARGET	= atmega644p
OPTIMIZE	= -Os

DEFS	+= -DF_CPU=12000000
#DEFS	+= -DTFT_VERTICALSCROLLING
LIBS	+= -lm
INCS	+= -I.

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

LOG	= ~/.make_avr.log

include $(SUBDIRS:%=%/Makefile)
include ../Makefile.inc
