# Size of image on each items
LIST_ITEM_IMAGE_SIZE	?= "(FONT_HEIGHT * 2)"
# Size of reserved area at top
LIST_TOP_RESERVED	?= 0
# Size of reserved area at bottom
LIST_BOTTOM_RESERVED	?= 0
# Sub menus stack level
LIST_STACK_SIZE		?= 5
CONFS	+= LIST_ITEM_IMAGE_SIZE LIST_TOP_RESERVED LIST_BOTTOM_RESERVED LIST_STACK_SIZE

# TFT vertical scrolling need to be enabled
TFT_VERTICAL_SCROLLING	= y

# File rules
NSRC	:= portraitlist.cpp
# landscapelist.cpp
SRC	+= $(NSRC:%=$(DIR)/%)
INCDIRS	+= $(DIR)
