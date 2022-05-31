
# Makefile for Divelink
# Author : Byunghun Hwang <bh.hwang@iae.re.kr>

# Makefile

OS := $(shell uname)

#Set Architecutre
ARCH := x86_64

#Compilers
CC := g++-8
GCC := gcc-8
LD_LIBRARY_PATH += -L./lib/x86_64
BUILDDIR		= ../../dist/
INCLUDE_DIR = -I../../include/ -I../../ -I../../include/3rdparty/
LD_LIBRARY_PATH += -L/usr/local/lib -L./lib/x86_64


# OS
ifeq ($(OS),Linux) #for Linux
	LDFLAGS = -Wl,--export-dynamic -Wl,-rpath=$(LD_LIBRARY_PATH)
	LDLIBS = -pthread -lboost_thread -lboost_chrono -lboost_system
	GTEST_LDLIBS = -lgtest
endif

$(shell mkdir -p $(BUILDDIR))

#if release(-O3), debug(-O0)
CXXFLAGS = -O3 -fPIC -Wall -std=c++1z -D__cplusplus=201703L

#custom definitions
CXXFLAGS += -D__MAJOR__=0 -D__MINOR__=0 -D__REV__=1
RM	= rm -rf

#directories

INCLUDE_FILES = ./include/
SOURCE_FILES = ./
INSTALL_DIR = /usr/local/bin/

# Make
divelink_m64:	$(BUILDDIR)divelink_m64.o \
				$(BUILDDIR)wl_m64.o \
				$(BUILDDIR)sync_uart.o 
		$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)divelink_m64.o:	./divelink_m64.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)wl_m64.o:	$(INCLUDE_DIR)device/waterlinked/m64.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)sync_uart.o:	$(INCLUDE_DIR)common/sync_uart.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

all : divelink_m64

deploy : FORCE
	cp $(BINDIR)
clean : FORCE 
		$(RM) $(BUILDDIR)*.o
FORCE : 