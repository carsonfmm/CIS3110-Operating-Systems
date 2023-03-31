CC = gcc
CFLAGS = -std=gnu99 -Wpedantic

make all: simcpu

simcpu.o: simcpu.c
	$(CC) $(CFLAGS) -c simcpu.c -o simcpu.o

simcpu: simcpu.o
	$(CC) $(CFLAGS) -o simcpu simcpu.o

clean:
	rm simcpu *.o