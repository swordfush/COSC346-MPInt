CC=clang++
CF=-g -Wall -pedantic -std=c++11

all : test

UInt32Vector.o : UInt32Vector.cpp UInt32Vector.hpp
	$(CC) $(CF) -c UInt32Vector.cpp

MPUInteger.o : MPUInteger.cpp MPUInteger.hpp UInt32Vector.hpp
	$(CC) $(CF) -c MPUInteger.cpp 

MPInteger.o : MPInteger.cpp MPInteger.hpp MPUInteger.hpp
	$(CC) $(CF) -c MPInteger.cpp

test : test.cpp MPUInteger.o UInt32Vector.o MPInteger.o
	$(CC) $(CF) -o test test.cpp MPUInteger.o  UInt32Vector.o MPInteger.o
	

clean:
	rm *.o
	rm test
