
# Author : Byunghun Hwang <bh.hwang@iae.re.kr>
# Usage : make ARCH=armhf
# Note : You should make with GCC/G++ version 8

# Makefile

OS := $(shell uname)

#Set Architecutre
ARCH := armhf
#ARCH := x86_64

CURRENT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
CURRENT_DIR_NAME := $(notdir $(patsubst %/,%,$(dir $(CURRENT_DIR))))
OPENEDGE_INCLUDE_DIR = $(CURRENT_DIR)/../../include/openedge

$(info $(CURRENT_DIR))


#Compilers
ifeq ($(ARCH),armhf)
	CC := /usr/bin/arm-linux-gnueabihf-g++-9
	GCC := /usr/bin/arm-linux-gnueabihf-gcc-9
	LD_LIBRARY_PATH += -L./lib/armhf
	OUTDIR		= $(CURRENT_DIR)/../../bin/armhf/
	BUILDDIR		= $(CURRENT_DIR)/../../bin/armhf/
	INCLUDE_DIR = -I./ -I$(CURRENT_DIR)/ -I$(CURRENT_DIR)/../../include/ -I$(CURRENT_DIR)/../../include/3rdparty/
	LD_LIBRARY_PATH += -L/usr/local/lib -L./lib/armhf -L$(CURRENT_DIR)/../../lib/armhf/
else
	CC := g++
	GCC := gcc
	LD_LIBRARY_PATH += -L./lib/x86_64
	OUTDIR		= $(CURRENT_DIR)/../../bin/x86_64/
	BUILDDIR		= $(CURRENT_DIR)/../../bin/x86_64/
	INCLUDE_DIR = -I./ -I$(CURRENT_DIR)/ -I$(CURRENT_DIR)/../../include/ -I$(CURRENT_DIR)/../../include/3rdparty/
	LD_LIBRARY_PATH += -L/usr/local/lib -L$(CURRENT_DIR)/../../lib/x86_64/
endif

# OS
ifeq ($(OS),Linux) #for Linux
	LDFLAGS = -Wl,--export-dynamic -Wl,-rpath=$(LD_LIBRARY_PATH)
	LDLIBS = -pthread -lrt -ldl -lm
	GTEST_LDLIBS = -lgtest
endif

$(shell mkdir -p $(OUTDIR))
$(shell mkdir -p $(BUILDDIR))

#if release(-O3), debug(-O0)
CXXFLAGS = -O3 -fPIC -Wall -std=c++17 -D__cplusplus=201703L

#custom definitions
CXXFLAGS += -D__MAJOR__=0 -D__MINOR__=0 -D__REV__=2
RM	= rm -rf

#directories
INCLUDE_FILES = $(CURRENT_DIR)/../../include
SOURCE_FILES = .


# Components
test.periodic.comp: $(BUILDDIR)test.periodic.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lmosquittopp -lmosquitto

$(BUILDDIR)test.periodic.o: $(CURRENT_DIR)/test.periodic.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

all : components
components : test.periodic.comp
deploy : FORCE
	cp $(BUILDDIR)*.task $(BINDIR)
clean : FORCE 
		$(RM) $(BUILDDIR)test.periodic.o $(BUILDDIR)sync_uart.o $(BUILDDIR)test.periodic.comp
FORCE : 