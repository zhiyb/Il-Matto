# RTOS portable
#RTOSPORT	?= ATMega644P
CONFS	+= RTOSPORT

#DEFS	+= -DGCC_MEGA_AVR=$(RTOSPORT)

# File rules
ifdef RTOSPORT
NSRC	:= croutine.c queue.c list.c tasks.c
NSRC	+= portable/MemMang/heap_4.c
NSRC	+= portable/GCC/$(RTOSPORT)/port.c
SRC	+= $(NSRC:%=$(DIR)/%)
INCDIRS	+= $(DIR)/include $(DIR)/portable/GCC/$(RTOSPORT)
endif
