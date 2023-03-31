CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

make all: dine holes

dine.o: dine.c
	$(CC) $(CFLAGS) -lpthread -c dine.c -o dine.o

dine: dine.o
	$(CC) $(CFLAGS) -lpthread -o dine dine.o

holes.o: holes.c
	$(CC) $(CFLAGS) -c holes.c -o holes.o

holes: holes.o
	$(CC) $(CFLAGS) -o holes holes.o

clean:
	rm dine holes *.o