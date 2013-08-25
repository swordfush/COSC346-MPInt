#ifndef _MPUINTEGER_HPP
#define _MPUINTEGER_HPP

#include "UInt32Vector.hpp"

/**
 * A multi-precision unsigned integer.
 */
struct MPUInteger
{
	/**
	 * Creates an instance with the 32 bit unsigned value specified.
	 */
	static MPUInteger *initWithUInt32Value(uint32_t value);

	~MPUInteger();

	/**
	 * Creates a copy of this unsigned integer.
	 */
	MPUInteger *copy() const;

	/**
	 * Adds x to this instance.
	 */
	void add(const MPUInteger *x);

	/**
	 * Subtracts the value of x from this instance.
	 * This instance must have a value that is greater than x.
	 */
	void subtract(const MPUInteger *x);
	
	/**
	 * Multiplies the value of this instance by x.
	 */
	void multiply(const MPUInteger *x);

	/**
	 * Divides this instance's value by x. x Can not be zero.
	 */
	MPUInteger *divide(const MPUInteger *x);

	/**
	 * Determines whether this instance and x are equivalent.
	 */
	bool equals(const MPUInteger *x) const;

	/** 
	 * Determines whether this instance is less than x.
	 */
	bool isLessThan(const MPUInteger *x) const;
	/**
	 * Determines whether this instance is less than x.
	 */
	bool isLessThanUInt32(uint32_t x) const;

	/**
	 * Determines whether this instance is zero.
	 */
	bool isZero() const;

	/**
	 * Adds x to this instance.
	 */
	void addUInt32(uint32_t x);
	/**
	 * Subtracts x from this instance.
	 * x cannot be greater than the value of this instance.
	 */
	void subtractUInt32(uint32_t x);
	/**
	 * Multiplies this instance by x.
	 */
	void multiplyUInt32(uint32_t x);
	/**
	 * Divides this instance by x.
	 * x cannot be zero.
	 */
	uint32_t divideUInt32(uint32_t x);

	private:
		MPUInteger(uint32_t value);
		MPUInteger(UInt32Vector *vector);

		static MPUInteger *initWithStolenDigits(MPUInteger *x);
		static MPUInteger *initWithZeroDigits(size_t n);

		MPUInteger *longDivide(const MPUInteger *divisor);
		MPUInteger *normalizedLongDivide(const MPUInteger *dividend, 
				const MPUInteger *divisor);
		static uint32_t estimateQuotientDigit(const MPUInteger *u, 
				const MPUInteger *v);

		void shiftLeft();

		UInt32Vector *digits;
};


#endif
