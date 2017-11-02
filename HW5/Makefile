#Usage: make [all|HW5server|HW5client]
#Use warning/wserver/wclient to show all compilation errors
#Use warning to print out all warnings from gcc
#Use all to compile both HW5server and HW5client
#Use HW5x to compile server or client

CC=gcc
CFLAGS=
LDLIBS=
#Put server/client dependencies here
SERVERDEPS=
CLIENTDEPS=

all: HW5server HW5client

HW5server: HW5server.c libsocket.o $(SERVERDEPS)
		$(CC) $(LDLIBS) -o $@ $^ $(CFLAGS)

HW5client: HW5client.c libsocket.o $(CLIENTDEPS)
		$(CC) $(LDLIBS) -o $@ $^ $(CFLAGS)

warning: CFLAGS+=-Wall -Werror
warning: all

wserver: CFLAGS+=-Wall -Werror
wserver: HW5server

wclient: CFLAGS+=-Wall -Werror
wclient: HW5client
