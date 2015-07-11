# Modified by zhiyb (Yubo Zhi)

TRG	= CC3000
SRC	= main.cpp menu.cpp event.cpp wlanfunc.cpp nvmemfunc.cpp socketfunc.cpp
#SRC	= patch/PatchProgrammer_DR_Patch.cpp
#SRC	= patch/PatchProgrammer_FW_Patch.cpp
SUBDIRS	= CC3000 tft-cpp common list rTouch

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

# Options
RTOUCH_SWAPXY	= n
LIST_ITEM_IMAGE_SIZE	= 0

LIBS	+= -lm

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
