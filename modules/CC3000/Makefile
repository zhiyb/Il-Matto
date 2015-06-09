# File rules
NSRC	:= cc3000_common.c evnt_handler.c hci.c netapp.c nvmem.c security.c socket.c wlan.c
SRC	+= $(NSRC:%=$(DIR)/CC3000HostDriver/%)
NSRC	:= Hardware/spi.c
SRC	+= $(NSRC:%=$(DIR)/%)
NINCS	:= CC3000HostDriver Hardware
INCDIRS	+= $(NINCS:%=$(DIR)/%)
