# Modified by zhiyb (Yubo Zhi)

PRG	= SDCard
SRC	= dirent.cpp errno.cpp fat32.cpp file.cpp main.cpp sd.cpp dac.c
LIB	= tft-cpp
LIBDIR	= ../lib
VERSION	= 0.0.1

PRGER	= usbasp
MCU_TARGET	= atmega644p
OPTIMIZE	= -Os

DEFS	+= -DF_CPU=12000000
LIBS	+=
INCS	+=

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

LOG	= ~/.make_avr.log

include ../Makefile.in
