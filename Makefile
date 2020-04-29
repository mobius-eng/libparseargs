CC=gcc
CFLAGS=-O3 -march=haswell
LD=ar

all: test lib

lib: libparseargs.a

test: testparse.o libparseargs.a
	$(CC) -o $@ $< -L. -lparseargs

libparseargs.a: parseargs.o
	$(LD) rcs $@ $^


%.o: %.c parseargs.h
	$(CC) -c -o $@ $(CFLAGS) $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f test

.PHONY: clean all lib


