# Modified by zhiyb (Yubo Zhi)

TRG	= rfm12b
SRC	= main.cpp phy_layer.c tcp_layer.c sampler.c pwm2.c
SRC	+= csma_0.c csma_1.c csma_p.c
SUBDIRS	= rfm12b uart0
#SUBDIRS	+= tft-cpp common 
OPTLEVEL	= 3

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

TFT_HARDWARE	= ili9341_parallel
TFT_PORT_CTRL	= A
TFT_PORT_DATA	= C

TFT_FONTS	= y

DEFS	+= -DUART0_BAUD=9600
LIBS	+= -lm

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
