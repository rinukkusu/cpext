CC=gcc
CFLAGS=-std=c99 -c -Wall
LDFLAGS=-lncurses

all: cpext

cpext: cpext.o
	$(CC) obj/cpext.o -o cpext $(LDFLAGS)

cpext.o: src/cpext.c
	$(CC) $(CFLAGS) src/cpext.c -o obj/cpext.o

clean:
	rm -rf obj/*o cpext

