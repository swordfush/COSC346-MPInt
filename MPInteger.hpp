#ifndef _MPINTEGER_HPP
#define _MPINTEGER_HPP


#include "MPMagnitude.hpp"

#include <string>


struct MPInteger
{
	static MPInteger *initWithString(const char *str);
	std::string description() const;

	~MPInteger();

	MPInteger *copy() const;

	MPInteger *add(const MPInteger *x) const;
	MPInteger *subtract(const MPInteger *x) const;
	MPInteger *multiply(const MPInteger *x) const;
	MPInteger *divide(const MPInteger *x) const;
	MPInteger *modulus(const MPInteger *x) const;

	bool isLessThan(const MPInteger *x) const;

	private:
		MPInteger(MPMagnitude *magnitude, bool isSigned);

		MPMagnitude *magnitude;
		bool isSigned;
};


#endif
