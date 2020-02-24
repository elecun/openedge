

# Makefile

OS := $(shell uname)

ifeq ($(OS), Darwin) #for Mac
	LDFLAGS = -dynamiclib
	LDLIBS = 
	INCLUDE_DIR = -I./include/
	LD_LIBRARY_PATH += -L/usr/local/opt/openssl/lib/
endif

ifeq ($(OS),Linux) #for Linux
	LDFLAGS = -Wl,--export-dynamic -pthread
	LDLIBS = 
	INCLUDE_DIR = -I./include/
	LD_LIBRARY_PATH += -L/usr/local/opt/openssl/lib/ -L/usr/local/lib
endif

DIRS=bin
$(shell mkdir -p $(DIRS))

CXX = g++
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
openedge:	$(OUTDIR)openedge.o \
		$(CXX) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(OUTDIR)$@ $^ $(LDLIBS)


#$(OUTDIR)format.o: $(INCLUDE_FILES)format.cc
#	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

#$(OUTDIR).o: $(INCLUDE_FILES).cpp
#	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@



all : openedge

clean : FORCE
		$(RM) $(OUTDIR)*.o $(OUTDIR)openedge
FORCE : 