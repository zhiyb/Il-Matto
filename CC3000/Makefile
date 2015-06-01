# Modified by zhiyb (Yubo Zhi)

TRG	= TFT-CPP
SRC	= main.cpp
#SRC	= patch/PatchProgrammer_DR_Patch.cpp
#SRC	= patch/PatchProgrammer_FW_Patch.cpp
SRC	+= Spi/spi.c
SUBDIRS	= CC3000HostDriver
INCDIRS	= CC3000HostDriver Spi
LIB	= tft-cpp common
LIBDIR	= ../lib

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

LIBS	+= -lm

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

include ../Makefile.defs
