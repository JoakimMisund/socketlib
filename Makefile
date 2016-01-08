CC = gcc
CCFLAGS = -c -g -Wall -Wextra
LC = gcc
LCFLAGS = -o

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

all: $(OBJECTS) test

test: test.o socketlib.o
	$(LC) $(LCFLAGS) test test.o socketlib.o

%.o: %.c
	$(CC) $(CCFLAGS) $<

.PHONY: clean

clean:
	rm -f $(OBJECTS) 
