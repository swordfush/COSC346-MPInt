#include <cstdlib>
#include <iostream>

#include "MPUInteger.hpp"

using namespace std;


int main(const int argc, const char **argv)
{
	cout << "Creating an integer with value 1234" << endl;
	MPUInteger *uint = MPUInteger::initWithUInt32Value(1234);
	uint->debug();

	cout << "Subtracting 234" << endl;
	MPUInteger *uint2 = MPUInteger::initWithUInt32Value(234);
	uint->subtract(uint2);
	uint->debug();

	cout << "Subtracting itself" << endl;
	delete uint2;
	uint2 = uint->copy();
	uint->subtract(uint2);
	uint->debug();

	cout << "Creating an integer using UINT32_MAX" << endl;
	delete uint;
	uint = MPUInteger::initWithUInt32Value(UINT32_MAX);
	uint->debug();

	cout << "Squaring it" << endl;
	delete uint2;
	uint2 = uint->copy();
	uint->multiply(uint2);
	uint->debug();

	cout << "Subtracting 1" << endl;
	delete uint2;
	uint2 = MPUInteger::initWithUInt32Value(1);
	uint->subtract(uint2);
	uint->debug();

	cout << "Adding 1" << endl;
	uint->add(uint2);
	uint->debug();



	delete uint;
	delete uint2;


	return EXIT_SUCCESS;
}
