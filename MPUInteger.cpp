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
	digits = vector->copy();
}

MPUInteger::~MPUInteger()
{
	delete digits;
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
	assert(this != x);

	const size_t length = x->digits->size();

	// Ensure we have at least the number of required digits
	this->digits->growToSize(length);

	uint64_t carry = 0;

	size_t i;
	
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

// Attempts to borrow for index i
// Note that this does not actually write to index i
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
	assert(this != x);

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
}

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
	assert(this != x);

	// Create a copy of the integer to read from
	MPUInteger *y = this->copy();

	// Zero out our current integer -- we'll be adding everything to it
	for (size_t i = 0; i < this->digits->size(); ++i)
	{
		this->digits->setItem(i, 0);
	}

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

MPUInteger *MPUInteger::divide(const MPUInteger *x)
{
	assert(false);

	MPUInteger *result = new MPUInteger(UInt32Vector::initWithSize(x->digits->size()));








	return NULL;
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


size_t MPUInteger::bitSize() const
{
	return 32 * this->digits->size();
}

int MPUInteger::bit(size_t index) const
{
	size_t uintIndex = index / 32;
	uint32_t mask = ((uint32_t)1) << (index % 32);

	return this->digits->item(uintIndex) & mask;
}

void MPUInteger::setBit(size_t index, int value)
{
	assert(value == 1 || value == 0);

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



void MPUInteger::debug() const
{
	this->digits->debug();
}



