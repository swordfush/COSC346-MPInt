#include "MPInteger.hpp"


MPInteger::MPInteger(MPUInteger *magnitude, bool isSigned)
{
	this->magnitude = magnitude;
	this->isSigned = isSigned;
}

MPInteger::~MPInteger()
{
	delete this->magnitude;
}

MPInteger *MPInteger::copy() const
{
	return new MPInteger(this->magnitude->copy(), this->isSigned);
}


MPInteger *MPInteger::add(const MPInteger *x) const
{
	MPInteger *result;

	if (this->isSigned == x->isSigned)
	{
		// Both integers have the same sign
		// x + y = x + y
		// -x + -y = -(x + y)
		// So we copy (including the sign) and add
		result = this->copy();
		result->magnitude->add(x->magnitude);
		return result;
	}
	else
	{
		// Both integers have different signs, so we need to perform subtraction
		// a + (-b)
		const MPInteger *a;
		const MPInteger *b;

		if (this->isSigned)
		{
			a = x;
			b = this;
		}
		else
		{
			a = this;
			b = x;
		}

		// If b > a then we'll run into an issue, use a - b = -(b - a)
		if (a->magnitude->isLessThan(b->magnitude))
		{
			result = b->copy();
			result->magnitude->subtract(a->magnitude);
			// b's sign is copied
		}
		else
		{
			result = a->copy();
			result->magnitude->subtract(b->magnitude);
			// a's sign is copied
		}
	}

	return result;
}

MPInteger *MPInteger::subtract(const MPInteger *x) const
{
	// We'll use the identity x - y = x + -y = -(-x + y)

	// To prevent allocating a scratch instance, we'll temporarily break const 
	// qualifiers to switch the sign of x
	MPInteger *discardConst = (MPInteger*)x;

	discardConst->isSigned = !discardConst->isSigned;

	// Perform addition with -x
	MPInteger *result = this->add(x);

	// Switch it back...
	discardConst->isSigned = !discardConst->isSigned;

	return result;
}

MPInteger *MPInteger::multiply(const MPInteger *x) const
{
	MPInteger *result = this->copy();
	result->magnitude->multiply(x->magnitude);

	result->isSigned = this->isSigned ^ x->isSigned;

	return result;
}

MPInteger *MPInteger::divide(const MPInteger *x) const
{
	MPInteger *result = this->copy();
	result->magnitude->divide(x->magnitude);

	result->isSigned = this->isSigned ^ x->isSigned;

	return result;
}

MPInteger *MPInteger::modulus(const MPInteger *x) const
{
	MPUInteger *divisor = this->magnitude->copy();

	MPUInteger *mod = divisor->divide(x->magnitude);
	bool sign = this->isSigned ^ x->isSigned;

	delete divisor;
	return new MPInteger(mod, sign);
}

bool MPInteger::isLessThan(const MPInteger *x) const
{
	if (this->isSigned == x->isSigned)
	{
		if (this->isSigned)
		{
			// Both are negative
			// If the absolute value of x is less than this then this is 
			// 'more negative' and therefore less than x
			return x->magnitude->isLessThan(this->magnitude);
		}
		else
		{
			// Both are positive -- simply use unsigned check
			return this->magnitude->isLessThan(x->magnitude);
		}
	}
	else
	{
		// Both have different signs, so if this is negative then it is smaller
		return this->isSigned;
	}
}
