#ifndef _MPUINTEGER_HPP
#define _MPUINTEGER_HPP


#include "UInt32Vector.hpp"


struct MPUInteger
{
	static MPUInteger *initWithUInt32Value(uint32_t value);
	~MPUInteger();

	MPUInteger *copy() const;

	void add(const MPUInteger *x);
	void subtract(const MPUInteger *x);
	void multiply(const MPUInteger *x);
	MPUInteger *divide(const MPUInteger *x);

	bool equals(const MPUInteger *x) const;

	bool isLessThan(const MPUInteger *x) const;
	bool isLessThanUInt32(uint32_t x) const;

	bool isZero() const;

	void addUInt32(uint32_t x);
	void subtractUInt32(uint32_t x);
	void multiplyUInt32(uint32_t x);
	uint32_t divideUInt32(uint32_t x);

	void debug() const;

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
