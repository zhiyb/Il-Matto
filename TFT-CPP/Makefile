# Modified by zhiyb (Yubo Zhi)

TRG	= TFT-CPP
SRC	= main.cpp
LIB	= tft-cpp capture common
LIBDIR	= ../lib

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

# TFT port settings
TFT_PORT_CTRL	= A
TFT_PORT_DATA	= C
# Vertical scrolling enable
TFT_VERTICALSCROLLING	= n

LIBS	+= -lm

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

include ../Makefile.defs
