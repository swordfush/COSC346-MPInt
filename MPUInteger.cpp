#include "MPUInteger.hpp"

#include <cassert>


#define BASE (uint64_t)UINT32_MAX


MPUInteger *MPUInteger::initWithUInt32Value(uint32_t value)
{
	return new MPUInteger(value);
}

MPUInteger *MPUInteger::initWithStolenDigits(MPUInteger *x)
{
	UInt32Vector *digits = x->digits;
	x->digits = NULL;

	return new MPUInteger(digits);
}

MPUInteger *MPUInteger::initWithZeroDigits(size_t n)
{
	return new MPUInteger(UInt32Vector::initWithSize(n));
}

MPUInteger::MPUInteger(uint32_t value)
{
	digits = UInt32Vector::initWithSize(1);
	digits->setItem(0, value);
}

MPUInteger::MPUInteger(UInt32Vector *vector)
{
	digits = vector;
}

MPUInteger::~MPUInteger()
{
	if (digits != NULL)
	{
		delete digits;
	}
}


MPUInteger *MPUInteger::copy() const
{
	return new MPUInteger(this->digits->copy());
}


// TODO: Use bit masks instead
#define LOW(x) ((x) % BASE)
#define HIGH(x) ((x) / BASE)


void MPUInteger::add(const MPUInteger *x)
{
	// We cannot add the same instance to itself
	// Instead we'll copy it then delete the copy
	if (this == x)
	{
		MPUInteger *copy = this->copy();
		this->add(copy);
		delete copy;
		return;
	}

	const size_t length = x->digits->size();

	// Ensure we have at least the number of required digits
	this->digits->growToSize(length);

	uint64_t carry = 0;

	size_t i;
	
	// Add all digits together, and move the carry on to the
	// next addition
	for (i = 0; i < length; ++i)
	{
		uint64_t result = (uint64_t)this->digits->item(i)
			+ (uint64_t)x->digits->item(i)
			+ carry;
		this->digits->setItem(i, LOW(result));
		carry = HIGH(result);

		assert(carry <= 1);
	}

	// We may still have carry, so we continue to add that
	while (carry > 0)
	{
		this->digits->growToSize(i + 1);

		uint64_t result = carry + (uint64_t)this->digits->item(i);
		this->digits->setItem(i, LOW(result));
		carry = HIGH(result);

		assert(carry <= 1);

		++i;
	}
}

void MPUInteger::addUInt32(uint32_t x)
{
	uint64_t carry = x;

	size_t i = 0;
	
	while (carry > 0)
	{
		this->digits->growToSize(i + 1);

		uint64_t result = carry + (uint64_t)this->digits->item(i);
		this->digits->setItem(i, LOW(result));
		carry = HIGH(result);

		assert(carry <= 1);

		++i;
	}
}

// Attempts to borrow for index i
// Note that this does not actually write to index i
// It also assumes that it will be able to borrow eventually
static void borrow(UInt32Vector *vector, size_t i)
{
	uint32_t nextValue = vector->item(i + 1);

	// If we are unable to borrow from the next index, recursively borrow 
	// from higher indices
	if (nextValue == 0)
	{
		borrow(vector, i + 1);
		nextValue = (uint32_t)BASE;
	}

	vector->setItem(i + 1, nextValue - 1);
}

void MPUInteger::subtract(const MPUInteger *x)
{
	if (this == x)
	{
		// Delete this instances' value and replace it with 0
		delete this->digits;
		this->digits = UInt32Vector::initWithSize(1);

		return;
	}

	assert(!(this->isLessThan(x)));

	const size_t length = x->digits->size();

	for (size_t i = 0; i < length; ++i)
	{
		uint64_t value = (uint64_t)this->digits->item(i);
		uint64_t xValue = (uint64_t)x->digits->item(i);

		// We may need to borrow from the next digit
		if (xValue > value)
		{
			borrow(this->digits, i);
			value += BASE;
		}

		assert(HIGH(value - xValue) == 0);
		this->digits->setItem(i, (uint32_t)LOW(value - xValue));
	}

	this->digits->discardLeadingZeros();
}

void MPUInteger::subtractUInt32(uint32_t x)
{
	assert(!this->isLessThanUInt32(x));
	
	uint64_t value = (uint64_t)this->digits->item(0);
	uint64_t xValue = (uint64_t)x;

	// We may need to borrow from the next digit
	if (xValue > value)
	{
		borrow(this->digits, 0);
		value += BASE;
	}

	assert(HIGH(value - xValue) == 0);
	this->digits->setItem(0, LOW(value - xValue));

	this->digits->discardLeadingZeros();
}

// Adds LOW(value) at the index specified, and HIGH(value) to the next index
static void addAtIndex(UInt32Vector *vector, size_t index, uint64_t value)
{
	// Ensure that the indices we'll be using exist
	vector->growToSize(index + 1);	

	uint64_t result = LOW(value) + (uint64_t)vector->item(index);
	vector->setItem(index, LOW(result));

	uint64_t carry = HIGH(value) + HIGH(result);

	// Continue adding the carry 
	while (carry != 0)
	{
		++index;

		vector->growToSize(index + 1);

		result = carry + (uint64_t)vector->item(index);
		vector->setItem(index, LOW(result));
		carry = HIGH(result);
	}
}

void MPUInteger::multiply(const MPUInteger *x)
{
	if (this == x)
	{
		MPUInteger *copy = this->copy();
		this->multiply(copy);
		delete copy;
		return;
	}

	// In the case where this is zero, we can end up with many leading zeros 
	// using the algorithm below -- we'll handle this case specially
	if (this->isZero() || x->isZero())
	{
		delete this->digits;
		this->digits = UInt32Vector::initWithSize(1);
		return;
	}

	// Store this instances' value
	MPUInteger *y = new MPUInteger(this->digits);

	// Zero out our current integer -- we'll be adding everything to it
	this->digits = UInt32Vector::initWithSize(y->digits->size());

	// Multiply each pair of digits and add them to the result
	for (size_t i = 0; i < x->digits->size(); ++i)
	{
		// If the digit is 0 then we'll skip any multiplication
		if (x->digits->item(i) == 0) continue;

		for (size_t j = 0; j < y->digits->size(); ++j)
		{
			uint64_t result = (uint64_t)x->digits->item(i) 
				* (uint64_t)y->digits->item(j);

			addAtIndex(this->digits, i + j, result);
		}
	}

	delete y;
}

void MPUInteger::multiplyUInt32(uint32_t x)
{
	// Store this instances' value
	MPUInteger *y = new MPUInteger(this->digits);

	// Zero out our current integer -- we'll be adding everything to it
	this->digits = UInt32Vector::initWithSize(y->digits->size());

	// Multiply each digit with x and store the result in this instance
	for (size_t i = 0; i < y->digits->size(); ++i)
	{
		uint64_t result = (uint64_t)y->digits->item(i) * (uint64_t)x;

		addAtIndex(this->digits, i, result);
	}

	delete y;
}

bool MPUInteger::isZero() const
{
	for (size_t i = 0; i < this->digits->size(); ++i)
	{
		if (this->digits->item(i) != 0)
		{
			return false;
		}
	}

	return true;
}

MPUInteger *MPUInteger::divide(const MPUInteger *x)
{
	assert(!x->isZero());

	if (this == x)
	{
		// x / x = 1 r 0
		delete this->digits;
		this->digits = UInt32Vector::initWithSize(1);
		this->digits->setItem(0, 1);

		return MPUInteger::initWithUInt32Value(0);
	}
	else if (x->digits->size() == 1)
	{
		// If the divisor consists of a single digit we have the fast algorithm
		uint32_t rem = this->divideUInt32(x->digits->item(0));

		return new MPUInteger(rem);
	}
	else if (this->isLessThan(x))
	{
		// If the divisor is greater than the dividend then we simply have this as the remainder
		MPUInteger *remainder = new MPUInteger(this->digits);
		this->digits = UInt32Vector::initWithSize(1);
		return remainder;
	}
	else
	{
		// We'll have to perform long division
		return this->longDivide(x);
	}
}

MPUInteger *MPUInteger::longDivide(const MPUInteger *divisor)
{
	// The following method is the one specified by Knuth in The Art of Computer Programming, Volume 2.

	// Obtain the factor to normalize with
	uint64_t v1 = divisor->digits->item(divisor->digits->size() - 1);
	uint32_t norm = (uint32_t)(BASE / (v1 + 1)); // Cast will always work

	// The normalized dividend with an added digit (u_0)
	MPUInteger *normalizedDividend = new MPUInteger(this->digits); 
	normalizedDividend->digits->growToSize(normalizedDividend->digits->size() + 1);
	normalizedDividend->multiplyUInt32(norm);

	// The normalized divisor
	MPUInteger *normalizedDivisor = divisor->copy();
	normalizedDivisor->multiplyUInt32(norm);

	// D2 through 7
	MPUInteger *remainder = this->normalizedLongDivide(normalizedDividend,
			normalizedDivisor);

	// D8: Unnormalize
	remainder->divideUInt32((uint32_t)BASE);

	delete normalizedDividend;
	delete normalizedDivisor;

	this->digits->discardLeadingZeros();
	remainder->digits->discardLeadingZeros();

	return remainder;
}

uint32_t MPUInteger::estimateQuotientDigit(const MPUInteger *u, 
		const MPUInteger *v)
{
	const uint64_t u0 = u->digits->item(u->digits->size() - 1);
	const uint64_t u1 = u->digits->item(u->digits->size() - 2);
	const uint64_t u2 = u->digits->item(u->digits->size() - 3);
	const uint64_t v1 = v->digits->item(v->digits->size() - 1);
	const uint64_t v2 = v->digits->item(v->digits->size() - 2);

	// The guess at the quotient digit
	uint32_t q;
	uint64_t r;

	// Calculate our guess
	if (u0 == v1)
	{
		q = BASE - 1;	
		r = u0 + u1;
	}
	else
	{
		q = (uint32_t)((u0 * BASE + u1) / v1);
		r = (u0 * BASE + u1) % v1;
	}

	// Correct our guess
	while (r < BASE && q * v2 > BASE * r + u2)
	{
		--q;
		r += v1;
	}

	return q;
}

/**
 * Performs long division on the normalized multi-precision integers provided. 
 * It is assumed that this instance's digits have either been freed or stolen.
 * The quotient of the division is placed into this instance.
 *
 * @param dividend The dividend. It must have a leading zero digit u_0 as 
 *  described by Knuth.
 * @param divisor The divisor.
 * @return The remainder of the division. 
 *  Note that this will still be normalized.
 */
MPUInteger *MPUInteger::normalizedLongDivide(const MPUInteger *dividend, 
		const MPUInteger *divisor)
{
	const size_t n = divisor->digits->size();
	// len(u) = m + n + 1 => m = len(u) - n - 1
	const size_t m = dividend->digits->size() - n - 1;

	MPUInteger *remainder = new MPUInteger(UInt32Vector::initWithSize(divisor->digits->size() + 1));

	// Copy digits u_0 ... u_n-1 into the remainder
	// u_n will be copied when the loop starts
	for (size_t i = 0; i < n; ++i)
	{
		const size_t offset = dividend->digits->size() - n;
		remainder->digits->setItem(i, dividend->digits->item(offset + i));
	}

	// We'll create this alias to make things clear
	MPUInteger *quotient = this;

	quotient->digits = UInt32Vector::initWithSize(m + 1);

	for (size_t j = 0; j <= m; ++j)
	{
		// Bring the next digit down
		remainder->shiftLeft();
		size_t nextDigitIndex = dividend->digits->size() - n - j - 1;
		uint32_t nextDigit = dividend->digits->item(nextDigitIndex);
		remainder->digits->setItem(0, nextDigit);

		// D3: Estimate the quotient
		uint32_t q = estimateQuotientDigit(remainder, divisor);

		// D4: Long divide subtract and multiply
		MPUInteger *subtracted = divisor->copy();
		subtracted->multiplyUInt32(q);

		if (remainder->isLessThan(subtracted))
		{
			// D6: Add back
			--q;

			// Add the divisor
			remainder->add(divisor);

			// Truncate the added digit
			remainder->digits->setItem(remainder->digits->size() - 1, 0);
			remainder->digits->discardLeadingZeros();
		}
		else
		{
			// Continue D4
			remainder->subtract(subtracted);
		}

		delete subtracted;

		// D5: Set quotient digit
		quotient->shiftLeft();
		quotient->digits->setItem(0, q);
	}

	return remainder;
}


uint32_t MPUInteger::divideUInt32(uint32_t x)
{
	uint64_t carry = 0;
	uint64_t temp;

	const MPUInteger *copy = new MPUInteger(this->digits);
	this->digits = UInt32Vector::initWithSize(copy->digits->size());

	const size_t last = copy->digits->size() - 1;

	for (size_t i = 0; i < copy->digits->size(); ++i)
	{
		temp = carry * BASE + copy->digits->item(last - i);

		this->digits->setItem(last - i, temp / x);
		carry = temp % x;
	}

	delete copy;

	this->digits->discardLeadingZeros();

	return carry;
}


bool MPUInteger::equals(const MPUInteger *x) const
{
	size_t highest = this->digits->highestNonZeroIndex();
	size_t xHighest = x->digits->highestNonZeroIndex();

	if (highest != xHighest)
	{
		return false;
	}

	for (size_t i = 0; i < highest; ++i)
	{
		if (this->digits->item(i) != x->digits->item(i))
		{
			return false;
		}
	}

	return true;
}

bool MPUInteger::isLessThan(const MPUInteger *x) const
{
	size_t highest = this->digits->highestNonZeroIndex();
	size_t xHighest = x->digits->highestNonZeroIndex();

	if (highest != xHighest)
	{
		// One integer contains a higher signficant digit
		return highest < xHighest;
	}
	else
	{
		// The two integers both have their most sigificant digit in the same 
		// position. We need to scan backwards to find where the first 
		// difference between the two occurs

		for (size_t i = highest; i > 0; --i)
		{
			uint32_t value = this->digits->item(i);
			uint32_t xValue = x->digits->item(i);

			if (value != xValue)
			{
				// Test whether the other integer has a higher value for its digit
				return value < xValue;
			}
		}
	}

	return this->digits->item(0) < x->digits->item(0);
}

bool MPUInteger::isLessThanUInt32(uint32_t x) const
{
	for (size_t i = this->digits->size() - 1; i > 0; --i)
	{
		if (this->digits->item(i) != 0)
			return false;
	}

	return this->digits->item(0) < x;
}

void MPUInteger::shiftLeft()
{
	// A single shift left can be seen as multiplication by UINT32_MAX
	this->multiplyUInt32((uint32_t)BASE);
}
