CC=clang
CF=-g -Wall -pedantic -ansi -std=c89

all : main

UInt32Vector.o : UInt32Vector.c UInt32Vector.h
	$(CC) $(CF) -c UInt32Vector.c

main : main.c UInt32Vector.o
	$(CC) $(CF) -o main main.c UInt32Vector.o
	

clean:
	rm *.o
