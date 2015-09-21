CC=gcc
CFLAGS= -c -Wall
CFLAGS2= -lpthread -o

all: rw_test

rw_test: rw_test.o rw_serial.o serial.o
	$(CC) rw_test.o rw_serial.o serial.o $(CFLAGS2) rw_test

rw_test.o: rw_test.c rw_serial.h
	$(CC) $(CFLAGS) rw_test.c

rw_serial.o: rw_serial.c rw_serial.h serial.h
	$(CC) $(CFLAGS) rw_serial.c 

serial.o: serial.c serial.h
	$(CC) $(CFLAGS) serial.c

clean: 
	rm *.o rw_test
