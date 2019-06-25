SHELL='bash'

AIOROOT=$(PWD)/../
AIOFLAGS=-I $(AIOROOT)

TIMER_CMD=time

CXX=g++
CXXFLAGS = -std=gnu++17 $(AIOFLAGS)

SOURCES += $(wildcard *.cpp)
TARGETS := $(patsubst %.cpp, %, $(SOURCES))

all: $(TARGETS)

%: %.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	@rm -f $(TARGETS)
