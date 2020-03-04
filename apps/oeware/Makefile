
# Makefile for oeware application
# Author : Byunghun Hwang <bh.hwang@iae.re.kr>
# Usage : make ARCH=arm 

# Makefile

OS := $(shell uname)

#Set Architecutre
#ARCH := arm

#Compilers
ifeq ($(ARCH),arm)
	CC := /usr/bin/arm-linux-gnueabihf-g++
	GCC := /usr/bin/arm-linux-gnueabihf-gcc
else
	CC := g++
	GCC := gcc
endif

# OS
ifeq ($(OS),Linux) #for Linux
	LDFLAGS = -Wl,--export-dynamic -pthread
	LDLIBS = -L./lib
	INCLUDE_DIR = -I../../ -I../../include/
	LD_LIBRARY_PATH += -L/usr/local/opt/openssl/lib/ -L/usr/local/lib
endif

DIRS=bin
$(shell mkdir -p $(DIRS))

CXXFLAGS = -O3 -fPIC -Wall -std=c++17 -D__cplusplus=201703L

#custom definition
CXXFLAGS += -D__MAJOR__=0 -D__MINOR__=0 -D__REV__=1
RM	= rm -rf

#directories
OUTDIR		= ./bin/


INCLUDE_FILES = ./include/
SOURCE_FILES = ./
INSTALL_DIR = /usr/local/bin/

# Make

oeware:	$(OUTDIR)oeware.o
		$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(OUTDIR)$@ $^ $(LDLIBS)

$(OUTDIR)oeware.o: $(SOURCE_FILES)oeware.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@



all : oeware

clean : FORCE
		$(RM) $(OUTDIR)*.o $(OUTDIR)oeware
FORCE : 