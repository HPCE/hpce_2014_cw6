SHELL=/bin/bash

CPPFLAGS += -std=c++11 -W -Wall  -g
CPPFLAGS += -O3
CPPFLAGS += -I ../include

puzzles.o : *.hpp

../lib/libpuzzler.a : puzzles.o
	-mkdir -p ../lib
	ar crf ../lib/libpuzzler.a puzzles.o

all : ../lib/libpuzzler.a
