# Modified by zhiyb (Yubo Zhi)

TRG	= SDCard
SRC	= dirent.cpp errno.cpp fat32.cpp file.cpp main.cpp sd.cpp dac.c
SUBDIRS	= tft-cpp common

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
