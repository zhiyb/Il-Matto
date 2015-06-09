# Modified by zhiyb (Yubo Zhi)

TRG	= LEDMatrix
SRC	= ascii.cpp dirent.cpp display.cpp errno.cpp fat32.cpp file.cpp lcd.cpp led.cpp main.cpp sd.cpp uart.cpp apps.cpp

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000
OPTLEVEL	= s

DEFS	+= -DBAUD=9600
LIBS	+= -lm

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
