# Modified by zhiyb (Yubo Zhi)

TRG	= list
SRC	= main.cpp menu.cpp menufunc.cpp
SUBDIRS	= tft-cpp list rTouch common misc eemem request

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

# Options
RTOUCH_SWAPXY	= n
#LIST_ITEM_IMAGE_SIZE	= 0
#LIST_TOP_RESERVED	= 0
#LIST_BOTTOM_RESERVED	= 0
#LIST_STACK_SIZE	= 5

#DEFS	+= -DDEBUG
LIBS	+= -lm

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
