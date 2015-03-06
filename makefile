SHELL=/bin/bash

CPPFLAGS += -std=c++11 -W -Wall  -g
CPPFLAGS += -O3
CPPFLAGS += -I include

lib/libpuzzler.a : provider/*.cpp provider/*.hpp
	cd provider && $(MAKE) all

bin/% : src/%.cpp lib/libpuzzler.a
	-mkdir -p bin
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS) -Llib -lpuzzler

all : bin/execute_puzzle bin/create_puzzle_input bin/run_puzzle
