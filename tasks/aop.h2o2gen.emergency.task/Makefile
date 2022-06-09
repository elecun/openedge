
# Makefile for OpenEdge Software Framework for Application Gateway
# Author : Byunghun Hwang <bh.hwang@iae.re.kr>
# Usage : make ARCH=armhf
# Note : You should make with GCC/G++ version 8

# Makefile

OS := $(shell uname)

#Set Architecutre
ARCH := armhf
# ARCH := x86_64

#Compilers
ifeq ($(ARCH),armhf)
	CC := /usr/bin/arm-linux-gnueabihf-g++-8
	GCC := /usr/bin/arm-linux-gnueabihf-gcc-8
	LD_LIBRARY_PATH += -L./lib/armhf
	OUTDIR		= ./bin/armhf/
	BUILDDIR		= ./bin/armhf/
	INCLUDE_DIR = -I./ -I./include/ -I./include/3rdparty/
	LD_LIBRARY_PATH += -L/usr/local/lib -L./lib/armhf
else
	CC := g++
	GCC := gcc
	LD_LIBRARY_PATH += -L./lib/x86_64
	OUTDIR		= ./bin/x86_64/
	BUILDDIR		= ./bin/x86_64/
	INCLUDE_DIR = -I./ -I./include/ -I./include/3rdparty/
	LD_LIBRARY_PATH += -L/usr/local/lib -L./lib/x86_64
endif

# OS
ifeq ($(OS),Linux) #for Linux
	LDFLAGS = -Wl,--export-dynamic -Wl,-rpath=$(LD_LIBRARY_PATH)
	LDLIBS = -pthread -lrt -ldl -lm -lczmq -lzmq
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

INCLUDE_FILES = ./include/
SOURCE_FILES = ./
APP_SOURCE_FILES = ./apps/
EXAMPLE_SOURCE_FILES = ./examples/
TASK_SOURCE_FILES = ./tasks/
SERVICE_SOURCE_FILES = ./services/
SUPPORT_SOURCE_FILES = ./support/
INSTALL_DIR = /usr/local/bin/


# Tasks
aop.h2o2gen.emg.p1.task: $(BUILDDIR)aop.h2o2gen.emg.p1.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)aop.h2o2gen.emg.p1.task.o: $(SOURCE_FILES)aop.h2o2gen.emg.p1.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


all : tasks
tasks : aop.h2o2gen.emg.p1.task
deploy : FORCE
	cp $(BUILDDIR)*.task $(BINDIR)
clean : FORCE 
		$(RM) $(BUILDDIR)*.o $(BUILDDIR)*.task
FORCE : 