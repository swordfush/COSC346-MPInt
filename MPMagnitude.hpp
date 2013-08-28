#ifndef _MPUINTEGER_HPP
#define _MPUINTEGER_HPP

#include "UInt32Vector.hpp"

/**
 * A multi-precision unsigned integer.
 */
struct MPMagnitude
{
	/**
	 * Creates an instance with the 32 bit unsigned value specified.
	 */
	static MPMagnitude *initWithUInt32Value(uint32_t value);

	~MPMagnitude();

	/**
	 * Creates a copy of this unsigned integer.
	 */
	MPMagnitude *copy() const;

	/**
	 * Adds x to this instance.
	 */
	void add(const MPMagnitude *x);

	/**
	 * Subtracts the value of x from this instance.
	 * This instance must have a value that is greater than x.
	 */
	void subtract(const MPMagnitude *x);
	
	/**
	 * Multiplies the value of this instance by x.
	 */
	void multiply(const MPMagnitude *x);

	/**
	 * Divides this instance's value by x. x Can not be zero.
	 */
	MPMagnitude *divide(const MPMagnitude *x);

	/**
	 * Determines whether this instance and x are equivalent.
	 */
	bool equals(const MPMagnitude *x) const;

	/** 
	 * Determines whether this instance is less than x.
	 */
	bool isLessThan(const MPMagnitude *x) const;
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
		MPMagnitude(uint32_t value);
		MPMagnitude(UInt32Vector *vector);

		static MPMagnitude *initWithStolenDigits(MPMagnitude *x);
		static MPMagnitude *initWithZeroDigits(size_t n);

		MPMagnitude *longDivide(const MPMagnitude *divisor);
		MPMagnitude *normalizedLongDivide(const MPMagnitude *dividend, 
				const MPMagnitude *divisor);
		static uint32_t estimateQuotientDigit(const MPMagnitude *u, 
				const MPMagnitude *v);

		void shiftLeft();

		UInt32Vector *digits;
};


#endif
