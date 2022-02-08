CC=gcc
CFLAGS=-g
CCLIBS=-lpthread
BINS=main

all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(CCLIBS)

clean:
	rm -rf *.dSYM $(BINS)
