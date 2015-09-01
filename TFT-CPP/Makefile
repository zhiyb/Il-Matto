# Modified by zhiyb (Yubo Zhi)

TRG	= TFT-CPP
SRC	= main.cpp
SUBDIRS	= tft-cpp tft-capture common

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

# for ili9341
TFT_HARDWARE	= ili9341_parallel
#TFT_HARDWARE	= st7735s_serial4_wo

ifeq ($(TFT_HARDWARE), ili9341_parallel)
	SRC	+= test_320x240.cpp
	# TFT port settings
	TFT_PORT_CTRL	= A
	TFT_PORT_DATA	= C
else ifeq ($(TFT_HARDWARE), st7735s_serial4_wo)
	SRC	+= test_128x128.cpp
	# TFT port settings (Port D only, currently)
	#TFT_PORT	= D
endif
# Vertical scrolling enable
TFT_VERTICAL_SCROLLING	= y

LIBS	+= -lm

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
