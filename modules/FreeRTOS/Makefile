# RTOS portable
RTOSPORT	?= $(MCU_TARGET)
CONFS	+= RTOSPORT

#DEFS	+= -DGCC_MEGA_AVR=$(RTOSPORT)

# File rules
NSRC	:= croutine.c queue.c list.c tasks.c
NSRC	+= portable/MemMang/heap_4.c
NSRC	+= portable/GCC/$(RTOSPORT)/port.c
SRC	+= $(NSRC:%=$(DIR)/%)
INCDIRS	+= $(DIR)/include $(DIR)/portable/GCC/$(RTOSPORT)
