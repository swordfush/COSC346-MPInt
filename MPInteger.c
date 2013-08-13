#include "MPInteger.h"

#include <assert.h>
#include <string.h>

#include "UInt32Vector.h"




struct MPInteger 
{
	UInt32Vector parts;
	bool isSigned;
};


/* We can think of a 64 bit uint as two 32 bit uints. UINT32_MAX has all its bits set. If we cast it to a uint64_t then we obtain all those bits which correspond with the lowest uint32_t. We can therefore use the compliment of (uint64_t)UINT32_MAX to mask out the high uint32_t. This allows us to extract the carry from our operations. */
#define LOW_MASK = ((uint64_t)UINT32_MAX)
#define HIGH_MASK = (~LOW_MASK)


/**
 * Adds the two integers and stores the result in a. The sign of each 
 * integer is not taken into account.
 */
static void add(MPInteger a, const MPInteger b)
{
	size_t length = UInt32Vector_count(b->parts);

	size_t i;
	uint64_t carry = 0u;
	uint64_t result;

	/* Ensure that a contains at least as many parts as b */
	UInt32Vector_growToLength(a->parts, UInt32Vector_count(b->parts));

	/* Add all parts of b to those in a, factoring in the carry each round */
	for (i = 0u; i < length; ++i) 
	{
		result = (uint64_t)a->parts[i] + (uint64_t)b->parts[i] + carry; 
		a->parts[i] = result & LOW_MASK; 
		carry = (result & HIGH_MASK) >> 32;
	}
	
	/* Handle remaining carry */
	if (carry != 0)
	{
		/* If the two vectors have the same length then we need to allocate an extra part to hold the carry */
		if (UInt32Vector_count(a->parts) == length) 
		{
			UInt32Vector_growToLength(a->parts, length + 1);
		}

		a->parts[length] = carry;
	}
}

static void subtract(MPInteger a, const MPInteger b)
{

}

static void multiply(MPInteger a, const MPInteger b)
{

}

static void divide(MPInteger a, const MPInteger b)
{

}

static void modulus(MPInteger a, const MPInteger b)
{

}



MPInteger MPInteger_createWithString(const char *str)
{
	MPInteger integer = malloc(sizeof(struct MPInteger));

	MPInteger mpInt10 = MPInteger_createWithUInt32(10, false);
	MPInteger digitTemp;
	MPInteger shiftTemp;

	size_t i = 0;
	size_t length = strlen(str);
	char digit;

	if (str[i] == '-') 
	{
		++i;
		integer->isSigned = true;
	} 
	else 
	{
		if (str[i] == '+')
		{
			++i;
		} 
		integer->isSigned = false;
	}

	for (; i < length; ++i) 
	{
		digit = str[i];
		assert(digit >= '0' && digit <= '9');
		digit = str[i] - '0';

		digitTemp = MPInteger_createWithUInt32((uint32_t)digit, false);
		shiftTemp = MPInteger_multiply(integer, mpInt10);

		MPInteger_delete(integer);

		integer = MPInteger_add(shiftTemp, digitTemp);

		MPInteger_delete(digitTemp);
		MPInteger_delete(shiftTemp);
	}

	MPInteger_delete(mpInt10);
	
	return integer;
}

MPInteger MPInteger_createWithUInt32(uint32_t value, bool isSigned)
{
	MPInteger integer = malloc(sizeof(struct MPInteger));

	integer->parts = UInt32Vector_createWithUInt32(value);
	integer->isSigned = isSigned;
	
	return integer;
}

void MPInteger_delete(MPInteger integer)
{
	UInt32Vector_delete(integer->parts);
	free(integer);
}

static void reverse_string(char *str)
{
	char *start;
	char *end;
	char temp;

	start = str;
	end = start + (strlen(str) - 1);

	while (start < end) 
	{
		temp = *start;
		*start = *end;
		*end = temp;
			
		++start;
		--end;
	}
}

static bool is_zero(const MPInteger integer)
{
	size_t i;
	size_t pieceCount = UInt32Vector_count(integer->pieces);

	for (i = 0; i < pieceCount; ++i) 
	{
		if (integer->pieces[i] != 0u)
		{
			return false;
		}
	}

	return true;
}

/* XXX: Note that the returned string must be freed after use */
char *MPInteger_description(const MPInteger integer)
{
	/* To allocate the buffer for the description we will use the maximum length that the number could allow */

	/* Since UINT32_MAX will not be a multiple of 10, and we're using floored division we'll just add 1 instead of using the ceil function */
	const int MAX_DIGITS_PER_UINT32 = UINT32_MAX / 10 + 1;

	size_t stringBufferLength = 1
		+ UInt32Vector_count(integer->parts) * MAX_DIGITS_PER_UINT32;
	char *stringValue = malloc(stringBufferLength);

	MPInteger scratch;
	MPInteger tmpScratch;

	memset(stringValue, 0, stringBufferLength);

	scratch = integer;

	while (!is_zero(scratch)) 
	{
		scratch = 

	}




	reverse_string(stringValue);

	return stringValue;
}

MPInteger MPInteger_add(MPInteger integer, MPInteger other)
MPInteger MPInteger_subract(MPInteger integer, MPInteger other)
MPInteger MPInteger_multiply(MPInteger integer, MPInteger other)
MPInteger MPInteger_divide(MPInteger integer, MPInteger other)
MPInteger MPInteger_modulus(MPInteger integer, MPInteger other)

bool MPInteger_isLessThan(MPInteger integer, MPInteger other)
{
	
}
