CC = gcc
CCFLAGS = -c -g -Wall -Wextra
LD = gcc
LDFLAGS = 

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

all: $(OBJECTS)

%.o: %.c
	$(CC) $(CCFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(OBJECTS) 
