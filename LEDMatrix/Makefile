# Modified by zhiyb (Yubo Zhi)

PRG	= LEDMatrix
SRC	= ascii.cpp dirent.cpp display.cpp errno.cpp fat32.cpp file.cpp lcd.cpp led.cpp main.cpp sd.cpp uart.cpp apps.cpp
LIB	= 
LIBDIR	= ../lib
VERSION	= 0.0.1

PRGER	= usbasp
MCU_TARGET	= atmega644p
OPTIMIZE	= -Os

DEFS	+= -DF_CPU=12000000 -DBAUD=9600
LIBS	+=
INCS	+=

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

LOG	= ~/.make_avr.log

include ../Makefile.in
