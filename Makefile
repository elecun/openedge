
# Makefile for OpenEdge Software Framework for Application Gateway
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
	LDLIBS = -pthread
	GTEST_LDLIBS = -lgtest
	INCLUDE_DIR = -I./ -I./include/
	LD_LIBRARY_PATH += -L/usr/local/lib
endif

$(shell mkdir -p $(OUTDIR))

CXXFLAGS = -O3 -fPIC -Wall -std=c++17 -D__cplusplus=201703L

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
openedge:	$(OUTDIR)openedge.o
			$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(OUTDIR)$@ $^ $(LDLIBS)

oeware:	$(OUTDIR)oeware.o \
		$(OUTDIR)instance.o
		$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(OUTDIR)$@ $^ $(LDLIBS)

oeware_test:	$(OUTDIR)oeware_test.o
				$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(OUTDIR)$@ $^ $(LDLIBS) $(GTEST_LDLIBS)

# for oeware
$(OUTDIR)oeware.o: $(APP_SOURCE_FILES)oeware/oeware.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(OUTDIR)instance.o: $(APP_SOURCE_FILES)oeware/instance.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

$(OUTDIR)oeware_test.o: $(APP_SOURCE_FILES)oeware/oeware_test.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

#for example
simiple.task: $(OUTDIR)simple.task.o 
	$(CXX) $(LDFLAGS) -shared -o $(OUTDIR)$@ $^ $(LDLIBS) -ldl
$(OUTDIR)simple.task.o: $(EXAMPLE_SOURCE_FILES)simple.task/simple.task.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


all : openedge oeware
example : simple.task
test : oeware_test

clean : FORCE
		$(RM) $(OUTDIR)*.o $(OUTDIR)openedge $(OUTDIR)oeware
FORCE : 