CC=clang++
CF=-g -Wall -pedantic -std=c++11

all : test

UInt32Vector.o : UInt32Vector.cpp UInt32Vector.hpp
	$(CC) $(CF) -c UInt32Vector.cpp

MPMagnitude.o : MPMagnitude.cpp MPMagnitude.hpp UInt32Vector.hpp
	$(CC) $(CF) -c MPMagnitude.cpp 

MPInteger.o : MPInteger.cpp MPInteger.hpp MPMagnitude.hpp
	$(CC) $(CF) -c MPInteger.cpp

test : test.cpp MPMagnitude.o UInt32Vector.o MPInteger.o
	$(CC) $(CF) -o test test.cpp MPMagnitude.o  UInt32Vector.o MPInteger.o
	

clean:
	rm *.o
	rm test
