# Modified by zhiyb (Yubo Zhi)

TRG	= CC3000
SRC	= main.cpp
#SRC	= patch/PatchProgrammer_DR_Patch.cpp
#SRC	= patch/PatchProgrammer_FW_Patch.cpp
SUBDIRS	= CC3000 tft-cpp common

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

LIBS	+= -lm

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
