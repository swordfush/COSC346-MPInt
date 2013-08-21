#include "MPUInteger.hpp"

#include <cassert>



MPUInteger *MPUInteger::initWithUInt32Value(uint32_t value)
{
	return new MPUInteger(value);
}

MPUInteger::MPUInteger(uint32_t value)
{
	digits = UInt32Vector::initWithSize(1);
	digits->setItem(0, value);
}

// Claims ownership of vector
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
#define LOW(x) ((x) % (uint64_t)UINT32_MAX)
#define HIGH(x) ((x) / (uint64_t)UINT32_MAX)


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
		this->digits->setItem(i, (uint32_t)LOW(result));
		carry = HIGH(result);

		assert(carry <= 1);
	}

	// We may still have carry, so we continue to add that
	while (carry > 0)
	{
		this->digits->growToSize(i + 1);

		uint64_t result = carry + (uint64_t)this->digits->item(i);
		this->digits->setItem(i, (uint32_t)LOW(result));
		carry = HIGH(result);

		assert(carry <= 1);

		++i;
	}
}

void MPUInteger::addUInt32(uint32_t x)
{
	uint64_t carry = x;

	size_t i = 0;
	
	// We may still have carry, so we continue to add that
	while (carry > 0)
	{
		this->digits->growToSize(i + 1);

		uint64_t result = carry + (uint64_t)this->digits->item(i);
		this->digits->setItem(i, (uint32_t)LOW(result));
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
	uint32_t nextValue = vector->item(i+1);

	// If we are unable to borrow from the next index, recursively borrow 
	// from higher indices
	if (nextValue == 0)
	{
		borrow(vector, i + 1);
		nextValue = UINT32_MAX;
	}

	vector->setItem(i+1, nextValue - 1);
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
			value += (uint64_t)UINT32_MAX;
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
		value += (uint64_t)UINT32_MAX;
	}

	assert(HIGH(value - xValue) == 0);
	this->digits->setItem(0, (uint32_t)LOW(value - xValue));

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

		// If we still have carry when we're at the last index we'll need to grow
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

	// In the case where this is zero, we can end up with many leading zeros from the multiplication algorithm used below -- we'll handle this case specially
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
			uint64_t result = (uint64_t)x->digits->item(i) * (uint64_t)y->digits->item(j);

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
		delete this->digits;
		this->digits = UInt32Vector::initWithSize(1);
		this->digits->setItem(0, 1);

		return MPUInteger::initWithUInt32Value(0);
	}

	MPUInteger *remainder;

	assert(false);

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
		temp = carry * UINT32_MAX + copy->digits->item(last - i);

		this->digits->setItem(last - i, temp / x);
		carry = temp % x;
	}

	delete copy;

	this->digits->discardLeadingZeros();

	return carry;
}



static size_t highestNonZeroIndex(const UInt32Vector *vector)
{
	size_t size = vector->size();

	for (size_t i = size - 1; i > 0; --i)
	{
		if (vector->item(i) != 0)
		{
			return i;
		}
	}

	return 0;
}

bool MPUInteger::isLessThan(const MPUInteger *x) const
{
	size_t highest = highestNonZeroIndex(this->digits);
	size_t xHighest = highestNonZeroIndex(x->digits);

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


size_t MPUInteger::bitSize() const
{
	return 32 * this->digits->size();
}

int MPUInteger::bit(size_t index) const
{
	size_t uintIndex = index / 32;
	uint32_t mask = ((uint32_t)1) << (index % 32);

	return (this->digits->item(uintIndex) & mask) >> index;
}

void MPUInteger::setBit(size_t index, int value)
{
	assert(value <= 1);

	size_t uintIndex = index / 32;
	uint32_t mask = ((uint32_t)1) << (index % 32); 

	uint32_t uint = this->digits->item(uintIndex);

	if (value)
	{
		uint |= mask;
	}
	else
	{
		uint &= ~mask;
	}
	
	this->digits->setItem(uintIndex, uint);
}

void MPUInteger::shiftLeft()
{
	// A single shift left can be seen as multiplication by 2
	MPUInteger *mp2 = new MPUInteger(2);
	this->multiply(mp2);
	delete mp2;
}

void MPUInteger::debug() const
{
	this->digits->debug();
}



