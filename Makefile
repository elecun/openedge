
# Makefile for OpenEdge Software Framework for Application Gateway
# Author : Byunghun Hwang <bh.hwang@iae.re.kr>
# Usage : make ARCH=armhf
# Note : You should make with GCC/G++ version 8

# Makefile

OS := $(shell uname)

#Set Architecutre
#ARCH := armhf
ARCH := x86_64

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

# Make
openedge:	$(BUILDDIR)openedge.o
			$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(BUILDDIR)$@ $^ $(LDLIBS)

# utiltiy
testptp: $(BUILDDIR)testptp.o
		$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(BUILDDIR)$@ $^ $(LDLIBS) 
$(BUILDDIR)testptp.o:	$(SOURCE_FILES)experimental/testptp/testptp.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

# edge service engine
edge:	$(BUILDDIR)edge.o \
		$(BUILDDIR)edge_instance.o \
		$(BUILDDIR)task_manager.o \
		$(BUILDDIR)driver.o \
		$(BUILDDIR)profile.o \
		$(BUILDDIR)rt_timer.o
		$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(BUILDDIR)$@ $^ $(LDLIBS)

# edge application sources
$(BUILDDIR)edge.o:	$(APP_SOURCE_FILES)edge/edge.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)task_manager.o:	$(APP_SOURCE_FILES)edge/task_manager.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)edge_instance.o: $(APP_SOURCE_FILES)edge/instance.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


# edge include files
$(BUILDDIR)rt_trigger.o:	$(INCLUDE_FILES)openedge/core/rt_trigger.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)rt_timer.o:		$(INCLUDE_FILES)openedge/core/rt_timer.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)driver.o:	$(INCLUDE_FILES)openedge/core/driver.cc
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


# common files for task
$(BUILDDIR)dkm_dx3000.o:	$(INCLUDE_FILES)/support/device/dkm_dx3000.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)dkm_dx3000_native.o:	$(INCLUDE_FILES)/support/device/dkm_dx3000_native.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


############################ Tasks
simple.task: $(BUILDDIR)simple.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)simple.task.o: $(TASK_SOURCE_FILES)simple.task/simple.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

simple2.task: $(BUILDDIR)simple2.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)simple2.task.o: $(TASK_SOURCE_FILES)simple2.task/simple2.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

aop10t.pilot.task: $(BUILDDIR)aop10t.pilot.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)aop10t.pilot.task.o: $(TASK_SOURCE_FILES)aop10t.pilot.task/aop10t.pilot.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

#sys.mdns.manage.task
sys.mdns.manage.task: $(BUILDDIR)sys.mdns.manage.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -ltinyxml2
$(BUILDDIR)sys.mdns.manage.task.o: $(TASK_SOURCE_FILES)sys.mdns.manage.task/sys.mdns.manage.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

qual.dmr.task: $(BUILDDIR)qual.dmr.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)qual.dmr.task.o: $(TASK_SOURCE_FILES)qual.dmr.task/qual.dmr.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


# i2c sensor logging task for injection modeling prj
logger.task: $(BUILDDIR)logger.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -li2c
$(BUILDDIR)logger.task.o: $(TASK_SOURCE_FILES)logger.task/logger.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

uvlc.ag.control.task: $(BUILDDIR)uvlc.ag.control.task.o \
					$(BUILDDIR)dkm_dx3000.o 
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lczmq -lzmq -lmodbus
$(BUILDDIR)uvlc.ag.control.task.o: $(TASK_SOURCE_FILES)uvlc.ag.control.task/uvlc.ag.control.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


uvlc.control.task: $(BUILDDIR)uvlc.control.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lczmq -lzmq -lmosquittopp -lmosquitto
$(BUILDDIR)uvlc.control.task.o: $(TASK_SOURCE_FILES)uvlc.control.task/uvlc.control.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

agsys.manage.task: $(BUILDDIR)agsys.manage.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lczmq -lzmq -lmodbus
$(BUILDDIR)agsys.manage.task.o: $(TASK_SOURCE_FILES)agsys.manage.task/agsys.manage.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

modbusRTU.task: $(BUILDDIR)modbusRTU.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lczmq -lzmq
$(BUILDDIR)modbusRTU.task.o: $(TASK_SOURCE_FILES)modbusRTU.task/modbusRTU.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

# sysmon task
sysmon.task: $(BUILDDIR)sysmon.o \
			 $(BUILDDIR)info.o \
			 $(BUILDDIR)cpuload.o \
			 $(BUILDDIR)netload.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lczmq -lzmq
$(BUILDDIR)sysmon.o: $(TASK_SOURCE_FILES)sysmon.task/sysmon.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

procmanage.task: $(BUILDDIR)procmanage.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lczmq -lzmq
$(BUILDDIR)procmanage.o: $(TASK_SOURCE_FILES)procmanage.task/procmanage.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

pcan.mqtt.task: $(BUILDDIR)pcan.mqtt.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lczmq -lzmq -lmosquittopp -lmosquitto
$(BUILDDIR)pcan.mqtt.task.o: $(TASK_SOURCE_FILES)pcan.mqtt.task/pcan.mqtt.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

peak.pcan.mqtt.task: $(BUILDDIR)peak.pcan.mqtt.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lmosquittopp -lmosquitto
$(BUILDDIR)peak.pcan.mqtt.task.o: $(TASK_SOURCE_FILES)peak.pcan.mqtt.task/peak.pcan.mqtt.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

fenet.task: $(BUILDDIR)fenet.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lmosquittopp -lmosquitto
$(BUILDDIR)fenet.task.o: $(TASK_SOURCE_FILES)fenet.task/fenet.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

dx3000.control.task: $(BUILDDIR)dx3000.control.task.o \
				$(BUILDDIR)dkm_dx3000.o 
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lczmq -lzmq -lmosquittopp -lmosquitto -lmodbus
$(BUILDDIR)dx3000.control.task.o: $(TASK_SOURCE_FILES)dx3000.control.task/dx3000.control.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

dx3000.udp.control.task: $(BUILDDIR)dx3000.udp.control.task.o \
				$(BUILDDIR)dkm_dx3000_native.o 
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lczmq -lzmq -lmosquittopp -lmosquitto
$(BUILDDIR)dx3000.udp.control.task.o: $(TASK_SOURCE_FILES)dx3000.udp.control.task/dx3000.udp.control.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


# test1.task for unit testing
test1.task: $(BUILDDIR)test1.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)test1.task.o: $(TASK_SOURCE_FILES)test1.task/test1.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


# test2.task for unit testing
test2.task: $(BUILDDIR)test2.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)test2.task.o: $(TASK_SOURCE_FILES)test2.task/test2.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


# agw.manage.task (it only works for OSD3358-based AGW)
agw.manage.task: $(BUILDDIR)agw.manage.task.o \
				 $(BUILDDIR)agw.mqtt.o \
				 $(BUILDDIR)system.o \
				 $(BUILDDIR)network.o \
				 $(BUILDDIR)network_perf.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lmosquittopp -lmosquitto
$(BUILDDIR)agw.manage.task.o: $(TASK_SOURCE_FILES)agw.manage.task/agw.manage.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)agw.mqtt.o: $(TASK_SOURCE_FILES)agw.manage.task/agw.mqtt.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

# nport udp task
nport.udp.task: $(BUILDDIR)nport.udp.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)nport.udp.task.o: $(TASK_SOURCE_FILES)nport.udp.task/nport.udp.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

# moxa io(iologik 1212 device) task
moxa.io.service.task: $(BUILDDIR)moxa.io.service.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lmodbus -lmosquittopp -lmosquitto
$(BUILDDIR)moxa.io.service.task.o: $(TASK_SOURCE_FILES)moxa.io.service.task/moxa.io.service.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

# moxa analog service task
moxa.analog.service.task: $(BUILDDIR)moxa.analog.service.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lmodbus -lmosquittopp -lmosquitto
$(BUILDDIR)moxa.analog.service.task.o: $(TASK_SOURCE_FILES)moxa.analog.service.task/moxa.analog.service.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

# dx3000 motor service task
dx3000.motor.service.task: $(BUILDDIR)dx3000.motor.service.task.o \
						$(BUILDDIR)dkm_dx3000_native.o 
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lmosquittopp -lmosquitto
$(BUILDDIR)dx3000.motor.service.task.o: $(TASK_SOURCE_FILES)dx3000.motor.service.task/dx3000.motor.service.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

# uvlc control logic task
uvlc.control.logic.task: $(BUILDDIR)uvlc.control.logic.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lmosquittopp -lmosquitto
$(BUILDDIR)uvlc.control.logic.task.o: $(TASK_SOURCE_FILES)uvlc.control.logic.task/uvlc.control.logic.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

# Realtime performance test task
rt.perf.test.task: $(BUILDDIR)rt.perf.test.task.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lmosquittopp -lmosquitto
$(BUILDDIR)rt.perf.test.task.o: $(TASK_SOURCE_FILES)rt.perf.test.task/rt.perf.test.task.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
# $(BUILDDIR)manager.o: $(TASK_SOURCE_FILES)rt.perf.test.task/manager.cpp
# 	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
# $(BUILDDIR)const.o: $(TASK_SOURCE_FILES)rt.perf.test.task/const.cpp
# 	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


############################ Services

lsis.fenet.connector.service: $(BUILDDIR)lsis.fenet.connector.service.o \
							$(BUILDDIR)xgt.protocol.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) ./lib/$(ARCH)/libsockpp.a
$(BUILDDIR)lsis.fenet.connector.service.o: $(SERVICE_SOURCE_FILES)lsis.fenet.connector.service/lsis.fenet.connector.service.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)xgt.protocol.o: $(SERVICE_SOURCE_FILES)lsis.fenet.connector.service/xgt.protocol.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

mongodb.connector.service: $(BUILDDIR)mongodb.connector.service.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lbson-1.0 -lmongoc-1.0
$(BUILDDIR)mongodb.connector.service.o: $(SERVICE_SOURCE_FILES)mongodb.connector.service/mongodb.connector.service.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

mqtt.publisher.service: $(BUILDDIR)mqtt.publisher.service.o \
						$(BUILDDIR)mqtt.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lmosquittopp
$(BUILDDIR)mqtt.publisher.service.o: $(SERVICE_SOURCE_FILES)mqtt.publisher.service/mqtt.publisher.service.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)mqtt.o: $(SERVICE_SOURCE_FILES)mqtt.publisher.service/mqtt.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

modbus.rtu.service: $(BUILDDIR)modbus.rtu.service.o
	$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS) -lmodbus
$(BUILDDIR)modbus.rtu.service.o: $(SERVICE_SOURCE_FILES)modbus.rtu.service/modbus.rtu.service.cc
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

all : edge tasks services

test : testptp
tasks : simple.task simple2.task aop10t.pilot.task sys.mdns.manage.task qual.dmr.task sysmon.task procmanage.task uvlc.ag.control.task pcan.mqtt.task agsys.manage.task uvlc.contro.task fenet.mqtt.task
services : lsis.fenet.connector.service mongodb.connector.service mqtt.publisher.service modbus.rtu.service
deploy : FORCE
	cp $(BUILDDIR)*.task $(BUILDDIR)edge $(BINDIR)
clean : FORCE 
		$(RM) $(BUILDDIR)*.o $(BUILDDIR)openedge $(BUILDDIR)edge $(BUILDDIR)*.task $(BUILDDIR)*.service
FORCE : 
