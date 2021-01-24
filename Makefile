CC=gcc
CFLAGS=-Wall
PROJECT=time-server
LDFLAGS=-lm -lpthread -lwiringPi
DESTDIR=/usr/local/bin

all: $(PROJECT)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

$(PROJECT): $(patsubst %.c, %.o, $(wildcard *.c)) defs.h
	$(CC) $^ -o $@ $(LDFLAGS)

.PHONY: clean run install uninstall

clean:
	rm -f *.o $(PROJECT)