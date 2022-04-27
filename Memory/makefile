CC=gcc 
CFLAGS=-Wpedantic -std=gnu99 -Wall -g
SRC=src/

all: $(SRC)holes.c $(SRC)holes.h
	$(CC) $(CFLAGS) $(SRC)holes.c -lm -o holes

clean:
	rm -r holes holes.dSYM
