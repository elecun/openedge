
# Author : Byunghun Hwang <bh.hwang@iae.re.kr>

# Makefile

OS := $(shell uname)

#Set Architecutre
ARCH := arm

#Compilers
ifeq ($(ARCH),arm)
	CC := /usr/bin/arm-linux-gnueabihf-g++
	GCC := /usr/bin/arm-linux-gnueabihf-gcc
	LD_LIBRARY_PATH += -L./lib/arm
	OUTDIR		= ./bin/arm/
else
	CC := g++
	GCC := gcc
	LD_LIBRARY_PATH += -L./lib/x86_64
	OUTDIR		= ./bin/x86_64/
endif

# OS
ifeq ($(OS),Linux) #for Linux
	LDFLAGS = -Wl,--export-dynamic
	LDLIBS = -pthread -lrt
	GTEST_LDLIBS = -lgtest
	INCLUDE_DIR = -I./ -I./include/
	LD_LIBRARY_PATH += -L/usr/local/lib
endif

$(shell mkdir -p $(OUTDIR))

#CXXFLAGS = -O3 -fPIC -Wall -std=c++17 -D__cplusplus=201703L

#custom definition
CXXFLAGS += -D__MAJOR__=0 -D__MINOR__=0 -D__REV__=1
RM	= rm -rf

#directories

INCLUDE_FILES = ./include/
SOURCE_FILES = ./
APP_SOURCE_FILES = ./apps/
EXAMPLE_SOURCE_FILES = ./examples/
INSTALL_DIR = /usr/local/bin/

# Make
itimer:	$(OUTDIR)itimer.o
			$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(OUTDIR)$@ $^ $(LDLIBS)

# for itimer
$(OUTDIR)itimer.o: $(SOURCE_FILES)itimer4.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


all : itimer

clean : FORCE
		$(RM) $(OUTDIR)*.o $(OUTDIR)itimer
FORCE : 
