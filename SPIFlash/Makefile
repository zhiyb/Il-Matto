# Modified by zhiyb (Yubo Zhi)

TRG	= SPIFlash
SRC	= main.cpp spiflash.cpp
SUBDIRS	= tft-cpp uart0 common
INCDIRS	= .

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

RTOSPORT	= ATMega644P

TFT_HARDWARE	= ili9341_parallel
TFT_FONTS	= y

UART0_BAUD	= 1500000

LIBS	+= -lm

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
