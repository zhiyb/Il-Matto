# Modified by zhiyb (Yubo Zhi)

TRG	= MPU6050
SRC	= main.cpp
SUBDIRS	= tft-cpp MPU6050-cpp common misc

PRGER		= usbasp
MCU_TARGET	= atmega644p
MCU_FREQ	= 12000000

MPU6050_PORT	= B
MPU6050_PORT_SCL	= 6
MPU6050_PORT_SDA	= 7

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

SRCDIR	?= .
include $(SRCDIR)/../Makefile_AVR.defs
