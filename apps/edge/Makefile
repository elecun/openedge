
# Author : Byunghun Hwang <bh.hwang@iae.re.kr>
# Usage : make ARCH=armhf
# Note : You should make with GCC/G++ version 8

# Makefile

OS := $(shell uname)

#Set Architecutre
#ARCH := armhf
ARCH := x86_64

CURRENT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
CURRENT_DIR_NAME := $(notdir $(patsubst %/,%,$(dir $(CURRENT_DIR))))

#Compilers
ifeq ($(ARCH),armhf)
	CC := /usr/bin/arm-linux-gnueabihf-g++-8
	GCC := /usr/bin/arm-linux-gnueabihf-gcc-8
	LD_LIBRARY_PATH += -L./lib/armhf
	OUTDIR		= $(CURRENT_DIR)/../../bin/armhf/
	BUILDDIR		= $(CURRENT_DIR)/../../bin/armhf/
	INCLUDE_DIR = -I./ -I$(CURRENT_DIR)/ -I$(CURRENT_DIR)/../../include/ -I$(CURRENT_DIR)/../../include/3rdparty/
	LD_LIBRARY_PATH += -L/usr/local/lib -L./lib/armhf
else
	CC := g++
	GCC := gcc
	LD_LIBRARY_PATH += -L./lib/x86_64
	OUTDIR		= $(CURRENT_DIR)/../../bin/x86_64/
	BUILDDIR		= $(CURRENT_DIR)/../../bin/x86_64/
	INCLUDE_DIR = -I./ -I$(CURRENT_DIR) -I$(CURRENT_DIR)/../../include -I$(CURRENT_DIR)/../../include/3rdparty
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
CXXFLAGS += -D__MAJOR__=0 -D__MINOR__=0 -D__REV__=5
RM	= rm -rf

#directories

INCLUDE_FILES = $(CURRENT_DIR)/../../include
SOURCE_FILES = .


# edge service engine
edge:	$(BUILDDIR)edge.o \
		$(BUILDDIR)edge_instance.o \
		$(BUILDDIR)task_manager.o \
		$(BUILDDIR)driver.o \
		$(BUILDDIR)profile.o \
		$(BUILDDIR)rt_timer.o
		$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(BUILDDIR)$@ $^ $(LDLIBS)

# edge application sources
$(BUILDDIR)edge.o:	$(CURRENT_DIR)/edge.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)task_manager.o:	$(CURRENT_DIR)/task_manager.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)edge_instance.o: $(CURRENT_DIR)/instance.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

$(BUILDDIR)driver.o:	$(INCLUDE_FILES)/openedge/core/driver.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

# edge include files
$(BUILDDIR)rt_trigger.o:	$(INCLUDE_DIR)/openedge/core/rt_trigger.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)rt_timer.o:		$(INCLUDE_FILES)openedge/core/rt_timer.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)profile.o:	$(INCLUDE_FILES)openedge/core/profile.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)uuid.o:	$(INCLUDE_FILES)openedge/util/uuid.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)general.o:	$(INCLUDE_FILES)openedge/device/general.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)info.o:	$(INCLUDE_FILES)openedge/sys/info.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)cpu.o:	$(INCLUDE_FILES)openedge/sys/cpu.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)network.o:	$(INCLUDE_FILES)openedge/sys/network.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)network_perf.o:	$(INCLUDE_FILES)openedge/sys/network_perf.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)memory.o:	$(INCLUDE_FILES)openedge/sys/memory.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)system.o:	$(INCLUDE_FILES)openedge/sys/system.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


all : edge

deploy : FORCE
	cp $(BUILDDIR)*.task $(BUILDDIR)edge $(BINDIR)
clean : FORCE 
		$(RM) $(BUILDDIR)*.o $(BUILDDIR)edge $(BUILDDIR)*.task
FORCE : 