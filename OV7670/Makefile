# Modified by zhiyb (Yubo Zhi)

TRG	= OV7670
SRC	= main.cpp
LIB	= tft-cpp OV7670-cpp common
LIBDIR	= ../lib

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

OV7670_PORT_CTRL	?= D
OV7670_PORT_DATA	?= B

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

include ../Makefile.defs
