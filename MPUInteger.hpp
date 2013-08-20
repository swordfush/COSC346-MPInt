#ifndef _MPUINTEGER_HPP
#define _MPUINTEGER_HPP


#include "UInt32Vector.hpp"


struct MPUInteger
{
	static MPUInteger *initWithUInt32Value(uint32_t value);
	~MPUInteger();

	MPUInteger *copy() const;

	void add(const MPUInteger *x);
	// this must be greater than x
	void subtract(const MPUInteger *x);
	void multiply(const MPUInteger *x);
	MPUInteger *divide(const MPUInteger *x);

	bool isLessThan(const MPUInteger *x) const;

	void debug() const;

	bool isZero() const;

	uint32_t lowestBase10Digit() const;
	
	private:
		MPUInteger(uint32_t value);
		MPUInteger(UInt32Vector *vector);

		size_t bitSize() const;
		int bit(size_t index) const;
		void setBit(size_t index, int value);

		void shiftLeft();


		UInt32Vector *digits;

};


#endif
