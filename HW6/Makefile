#Usage: make all
#Use warning to print out all warnings from gcc

CC=gcc
CFLAGS=
LDLIBS=-lpthread

all:
	$(CC) HW6.c buffer.c $(LDLIBS) -o HW6.out $^ $(CFLAGS)

warning: CFLAGS+=-Wall -Werror
warning: all
