CC=gcc
CFLAGS=-Wall -std=gnu99
SOURCES=ish.c
EXECUTABLE=ish

all: $(EXECUTABLE)

$(EXECUTABLE):
	$(CC) $(CFLAGS) $(SOURCES) -o $@

clean:
	rm -f $(EXECUTABLE)
