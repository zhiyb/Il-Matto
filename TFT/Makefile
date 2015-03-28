# Modified by zhiyb (Yubo Zhi)

TRG	= TFT
SRC	= main.c
LIB	= tft
LIBDIR	= ../lib

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

DEFS	+= -DF_CPU=12000000
#DEFS	+= -DTFT_USE_PORT_BD
#DEFS	+= -DTFT_CHECKING
#DEFS	+= -DTFT_VERTICALSCROLLING

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

include ../Makefile.defs
