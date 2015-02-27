# Name of target controller 
MCU=atmega644p
MCU_PROGRAMMER=m644p
MCU_FREQ=12000000 

# Name of our project
PROJECTNAME=libtft-cpp

# Source files
PRJSRC=ascii.cpp tft.cpp

# Optimization level, 
OPTLEVEL=s

DEFS ?=	-DF_CPU=$(MCU_FREQ)

# compiler
override CFLAGS=-I. $(INC) -g -mmcu=$(MCU) $(DEFS) -O$(OPTLEVEL) \
	-Wall                                   \
	-funsigned-bitfields -funsigned-char    \
	-fpack-struct -fshort-enums             \
	-Wa,-ahlms=$(firstword                  \
	$(filter %.lst, $(<:.c=.lst)))
override CXXFLAGS=-I. $(INC) -g -mmcu=$(MCU) $(DEFS) -O$(OPTLEVEL) \
	-Wall                                   \
	-funsigned-bitfields -funsigned-char    \
	-fpack-struct -fshort-enums             \
	-Wa,-ahlms=$(firstword                  \
	$(filter %.lst, $(<:.cpp=.lst)))

# Archiver
ARFLAGS=rcs

##### executables ####
CC=avr-gcc
CXX=avr-g++
AR=avr-ar
REMOVE=rm -f

##### automatic target names ####
LIBTRG=$(PROJECTNAME).a

# List all object files we need to create
CFILES=$(filter %.c, $(PRJSRC))
CPPFILES=$(filter %.cpp, $(PRJSRC))
OBJDEPS=$(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

# Define all lst files.
LST=$(filter %.lst, $(OBJDEPS:.o=.lst))

.SUFFIXES : .cpp .c .o .h

.PHONY: clean

# Make targets:
all: $(LIBTRG)

$(LIBTRG): $(OBJDEPS) 
	$(AR) $(ARFLAGS) $(LIBTRG) $(OBJDEPS)

#### Generating object files ####
.c.o: 
	$(CC) $(CFLAGS) -c $< -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

#### Cleanup ####
clean:
	$(REMOVE) $(LIBTRG)
	$(REMOVE) $(OBJDEPS)
	$(REMOVE) $(LST)

