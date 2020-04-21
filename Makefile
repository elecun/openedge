
# Makefile for OpenEdge Software Framework for Application Gateway
# Author : Byunghun Hwang <bh.hwang@iae.re.kr>
# Usage : make ARCH=arm 

# Makefile

OS := $(shell uname)

#Set Architecutre
ARCH := arm

#Compilers
ifeq ($(ARCH),arm)
	CC := /usr/bin/arm-linux-gnueabihf-g++-8
	GCC := /usr/bin/arm-linux-gnueabihf-gcc-8
	LD_LIBRARY_PATH += -L./lib/arm
	OUTDIR		= ./bin/arm/
	TASK_OUTDIR		= ./bin/arm/task/
else
	CC := g++
	GCC := gcc
	LD_LIBRARY_PATH += -L./lib/x86_64
	OUTDIR		= ./bin/x86_64/
	TASK_OUTDIR		= ./bin/x86_64/task/
endif

# OS
ifeq ($(OS),Linux) #for Linux
	LDFLAGS = -Wl,--export-dynamic
	LDLIBS = -pthread -lrt -ldl
	GTEST_LDLIBS = -lgtest
	INCLUDE_DIR = -I./ -I./include/
	LD_LIBRARY_PATH += -L/usr/local/lib
endif

$(shell mkdir -p $(OUTDIR))
$(shell mkdir -p $(TASK_OUTDIR))

#if release(-O3), debug(-O0)
CXXFLAGS = -O3 -fPIC -Wall -std=c++17 -D__cplusplus=201703L

#custom definition
CXXFLAGS += -D__MAJOR__=0 -D__MINOR__=0 -D__REV__=2
RM	= rm -rf

#directories


INCLUDE_FILES = ./include/
SOURCE_FILES = ./
APP_SOURCE_FILES = ./apps/
EXAMPLE_SOURCE_FILES = ./examples/
TASK_SOURCE_FILES = ./tasks/
SERVICE_SOURCE_FILES = ./services/
INSTALL_DIR = /usr/local/bin/

# Make
openedge:	$(OUTDIR)openedge.o
			$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(OUTDIR)$@ $^ $(LDLIBS)

oeware:	$(OUTDIR)oeware.o \
		$(OUTDIR)instance.o
		$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(OUTDIR)$@ $^ $(LDLIBS)

oeware_test:	$(OUTDIR)oeware_test.o
				$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(OUTDIR)$@ $^ $(LDLIBS) $(GTEST_LDLIBS)

# edge service engine
edge:	$(OUTDIR)edge.o \
		$(OUTDIR)edge_instance.o \
		$(OUTDIR)task_manager.o \
		$(OUTDIR)driver.o \
		$(OUTDIR)profile.o \
		$(OUTDIR)rt_timer.o
		$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(OUTDIR)$@ $^ $(LDLIBS)

#
# edge service engine
#
$(OUTDIR)edge.o: $(APP_SOURCE_FILES)edge/edge.cc
				$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(OUTDIR)task_manager.o: $(APP_SOURCE_FILES)edge/task_manager.cc
				$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(OUTDIR)edge_instance.o: $(APP_SOURCE_FILES)edge/instance.cc
				$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


#openedge base
$(OUTDIR)rt_trigger.o: $(INCLUDE_FILES)openedge/core/rt_trigger.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

# include
$(OUTDIR)rt_timer.o: $(INCLUDE_FILES)openedge/core/rt_timer.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

############################ Tasks
simple.task: $(OUTDIR)simple.task.o
	$(CC) $(LDFLAGS) -shared -o $(OUTDIR)$@ $^ $(LDLIBS) -ldl
$(OUTDIR)simple.task.o: $(TASK_SOURCE_FILES)simple.task/simple.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

simple2.task: $(OUTDIR)simple2.task.o
	$(CC) $(LDFLAGS) -shared -o $(OUTDIR)$@ $^ $(LDLIBS) -ldl
$(OUTDIR)simple2.task.o: $(TASK_SOURCE_FILES)simple2.task/simple2.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

plc.daq.task: $(OUTDIR)plc.daq.task.o
	$(CC) $(LDFLAGS) -shared -o $(OUTDIR)$@ $^ $(LDLIBS) -ldl
$(OUTDIR)plc.daq.task.o: $(TASK_SOURCE_FILES)plc.daq.task/plc.daq.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

############################ Services
plc.general.service: $(OUTDIR)plc.general.service.o
	$(CC) $(LDFLAGS) -shared -o $(OUTDIR)$@ $^ $(LDLIBS) -ldl
$(OUTDIR)plc.general.service.o: $(SERVICE_SOURCE_FILES)plc.general.service/plc.general.service.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

fenet.connector.service: $(OUTDIR)fenet.connector.service.o
	$(CC) $(LDFLAGS) -shared -o $(OUTDIR)$@ $^ $(LDLIBS) -ldl
$(OUTDIR)fenet.connector.service.o: $(SERVICE_SOURCE_FILES)fenet.connector.service/fenet.connector.service.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


############################ Openedge Cores
$(OUTDIR)driver.o:	$(INCLUDE_FILES)openedge/core/driver.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(OUTDIR)profile.o:	$(INCLUDE_FILES)openedge/core/profile.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(OUTDIR)uuid.o:	$(INCLUDE_FILES)openedge/util/uuid.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


all : edge
test : oeware_test
tasks : simple.task simple2.task plc.daq.task
services : plc.general.service
clean : FORCE
		$(RM) $(OUTDIR)*.o $(OUTDIR)openedge $(OUTDIR)edge $(OUTDIR)*.task $(OUTDIR)*.service
FORCE : 