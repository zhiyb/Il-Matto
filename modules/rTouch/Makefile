# Data valid variance (maximum)
RTOUCH_DELTA	?= 24
# Averager depth
RTOUCH_AVERAGER	?= 16
# Press / move threshold
RTOUCH_MOVETH	?= 16
CONFS	+= RTOUCH_DELTA RTOUCH_AVERAGER RTOUCH_MOVETH

# If x and y connection are reversed
CONFS_D	+= RTOUCH_SWAPXY
# FAST: no checks etc
CONFS_D	+= RTOUCH_SAFE

# File rules
NSRC	:= rtouch.cpp ts_calibrate.c
SRC	+= $(NSRC:%=$(DIR)/%)
INCDIRS	+= $(DIR)
