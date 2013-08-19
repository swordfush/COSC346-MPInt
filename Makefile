CC=clang++
CF=-g -Wall -pedantic -std=c++11

all : main

UInt32Vector.o : UInt32Vector.cpp UInt32Vector.hpp
	$(CC) $(CF) -c UInt32Vector.cpp

MPUInteger.o : MPUInteger.cpp MPUInteger.hpp UInt32Vector.hpp
	$(CC) $(CF) -c MPUInteger.cpp 

MPInteger.o : MPInteger.cpp MPInteger.hpp MPUInteger.hpp
	$(CC) $(CF) -c MPInteger.cpp

main : main.cpp MPUInteger.o UInt32Vector.o MPInteger.o
	$(CC) $(CF) -o main main.cpp MPUInteger.o  UInt32Vector.o MPInteger.o
	

clean:
	rm *.o
	rm main
