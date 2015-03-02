LIBNAME		= librTouch
LIBSRC		= rtouch.cpp ts_calibrate.c

LIB		= tft-cpp common
LIBS		+= -lm

MCU_TARGET	?= atmega644p
MCU_FREQ	?= 12000000 

OPTIMIZE	?= -Os
DEFS		?= -DF_CPU=$(MCU_FREQ)

include ../Makefile.in
