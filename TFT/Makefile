# Modified by zhiyb (Yubo Zhi)

TRG	= TFT
SRC	= main.c
SUBDIRS	= tft

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

#DEFS	+= -DTFT_USE_PORT_BD
#DEFS	+= -DTFT_CHECKING
#DEFS	+= -DTFT_VERTICALSCROLLING

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
