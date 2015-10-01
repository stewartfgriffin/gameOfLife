P=gameOfLife
OBJECTS=parseArgs.o point.o config.o grid.o
CFLAGS= -g -Wall -Werror -o3 -std=gnu11
LDLIBS=
CC=gcc

$(P): $(OBJECTS)
