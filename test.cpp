#include <cstdlib>
#include <iostream>

#include "MPInteger.hpp"

using namespace std;

int main(int argc, const char **argv)
{

	if (argc != 4)
	{
		cerr << "Invalid args:"
			<< "\n\t[1]\t" << argv[1]
			<< "\n\t[2]\t" << argv[2]
			<< "\n\t[3]\t" << argv[3] 
			<< endl;
		cerr << "Expected: <integer> <operator> <integer>" << endl;
		return EXIT_FAILURE;
	}

	MPInteger *a = MPInteger::initWithString(argv[1]);
	MPInteger *b = MPInteger::initWithString(argv[3]);

	MPInteger *result;

	switch (argv[2][0])
	{
		case '+':
			result = a->add(b);
			break;
		case '-':
			result = a->subtract(b);
			break;
		case '*':
			result = a->multiply(b);
			break;
		case '/':
			result = a->divide(b);
			break;
		case '%':
			result = a->modulus(b);
			break;

		default:
			return EXIT_FAILURE;
	}

	cout << result->description() << std::endl;
	result->debug();
	delete result;
	delete a;
	delete b;

	return EXIT_SUCCESS;
}
