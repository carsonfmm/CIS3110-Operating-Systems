CC = gcc
CFLAGS = -std=c99 -Wall -D_DEFAULT_SOURCE  -pedantic

make all: FSreport

FSreport.o: FSreport.c
	$(CC) $(CFLAGS) -lm -c FSreport.c -o FSreport.o

FSreport: FSreport.o
	$(CC) $(CFLAGS) -lm -o FSreport FSreport.o

clean:
	rm FSreport *.o