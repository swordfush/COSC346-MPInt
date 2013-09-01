//
//  MPMagnitude.m
//  MPInteger
//
//  Created by Stuart Johnston on 1/09/13.
//  Copyright (c) 2013 Stuart Johnston. All rights reserved.
//

#import "MPMagnitude.h"

@implementation MPMagnitude

#define BASE (MPDLimb)MPLIMB_MAX

#define LOW(x) ((x) % BASE)
#define HIGH(x) ((x) / BASE)

- (id)initWithLimbs:(MPLimbVector *)limbVector {
    self = [super init];
    if (self) {
        limbs = limbVector;
    }
    return self;
}

- (id)initWithLimbValue:(MPLimb)value {
    self = [self initWithLimbs:[[MPLimbVector alloc] initWithSize:1]];
    if (self) {
        [limbs setItem:0 toValue:value];
    }
    return self;
}

- (id)initWithZeroedLimbs:(NSUInteger)numberOfLimbs {
    return [self initWithLimbs:[[MPLimbVector alloc] initWithSize:numberOfLimbs]];
}

- (MPMagnitude *)copy {
    return [[MPMagnitude alloc] initWithLimbs:[limbs copy]];
}

- (MPLimb)limb:(NSUInteger)index {
    return [limbs item:index];
}

- (NSUInteger)numberOfLimbs {
    return [limbs size];
}

- (void)add:(MPMagnitude *)x {
    // We cannot add the same instance to itself
	// Instead we'll copy it then delete the copy
	if (self == x) {
		MPMagnitude *copy = [self copy];
		[self add:copy];
        return;
	}
    
	const NSUInteger length = [x numberOfLimbs];
    
	// Ensure we have at least the number of required digits
	[limbs growToSize:length];
    
	MPDLimb carry = 0;
    
	NSUInteger i;
	
	// Add all digits together, and move the carry on to the
	// next addition
	for (i = 0; i < length; ++i) {
		MPDLimb result = (MPDLimb)[self limb:i]
                + (MPDLimb)[x limb:i]
                + carry;
        [limbs setItem:i toValue:LOW(result)];
		carry = HIGH(result);
        
		NSAssert(carry <= 1, @"The carry was higher than it should ever be during addition");
	}
    
	// We may still have carry, so we continue to add that
	while (carry > 0) {
        [limbs growToSize:i + 1];
        
		MPDLimb result = carry + (MPDLimb)[self limb:i];
        [limbs setItem:i toValue:LOW(result)];
		carry = HIGH(result);
        
		assert(carry <= 1);
        
		++i;
	}
}

// Attempts to borrow for index i
// Note that this does not actually write to index i
// It also assumes that it will be able to borrow eventually
- (void)borrowForIndex:(NSUInteger)i {
	MPLimb nextValue = [self limb:i + 1];
    
	// If we are unable to borrow from the next index, recursively borrow
	// from higher indices
	if (nextValue == 0) {
        [self borrowForIndex:i + 1];
		nextValue = BASE;
	}
    
    // Subtract the next index
    [limbs setItem:i + 1 toValue:nextValue - 1];
}

- (void)subtract:(MPMagnitude *)x {
    if (self == x) {
		// x - x = 0
        limbs = [[MPLimbVector alloc] initWithSize:1];
        
		return;
	}
    
	NSAssert(![self isLessThan:x], @"Attempted to subtract a magnitude larger than the minuend");
    
	const NSUInteger length = [x numberOfLimbs];
    
	for (NSUInteger i = 0; i < length; ++i)
	{
		MPDLimb value = [self limb:i];
		MPDLimb xValue = [x limb:i];
        
		// We may need to borrow from the next digit
		if (xValue > value)
		{
			[self borrowForIndex:i];
			value += BASE;
		}
        
		NSAssert(HIGH(value - xValue) == 0, @"Borrowing took place when it was not supposed to; the result %llu is larger than a limb can hold", HIGH(value - xValue));
        [limbs setItem:i toValue:(MPLimb)LOW(value - xValue)];
	}
    
    [limbs discardLeadingZeros];
}

// Adds LOW(value) at the index specified, and HIGH(value) to the next index
- (void)addDLimb:(MPDLimb)value atIndex:(NSUInteger)index {
	// Ensure that the indices we'll be using exist
    [limbs growToSize:index + 1];
    
	MPDLimb result = LOW(value) + (MPDLimb)[self limb:index];
    [limbs setItem:index toValue:LOW(result)];
    
	MPDLimb carry = HIGH(value) + HIGH(result);
    
	// Continue adding the carry
	while (carry != 0)
	{
		++index;
        
        [limbs growToSize:index + 1];
        
		result = carry + [self limb:index];
        [limbs setItem:index toValue:LOW(result)];
		carry = HIGH(result);
	}
}

- (void)multiply:(MPMagnitude *)x {
    if (self == x) {
		MPMagnitude *copy = [self copy];
		[self multiply:copy];
		return;
	}
    
	// In the case where this is zero, we can end up with many leading zeros
	// using the algorithm below -- we'll handle this case specially
	if ([self isZero] || [x isZero]) {
        limbs = [[MPLimbVector alloc] initWithSize:1];
		return;
	}
    
	// Store this instances' value
	MPMagnitude *y = [[MPMagnitude alloc] initWithLimbs:limbs];
    
	// Zero out our current integer -- we'll be adding everything to it
    limbs = [[MPLimbVector alloc] initWithSize:[y numberOfLimbs]];
    
	// Multiply each pair of digits and add them to the result
	for (NSUInteger i = 0; i < [x numberOfLimbs]; ++i) {
		// If the digit is 0 then we'll skip any multiplication
		if ([x limb:i] == 0) continue;
        
		for (NSUInteger j = 0; j < [y numberOfLimbs]; ++j) {
			MPDLimb result = (MPDLimb)[x limb:i] * (MPDLimb)[y limb:j];
            
            [self addDLimb:result atIndex:i + j];
		}
	}
}

- (MPMagnitude *)divide:(MPMagnitude *)x {
    NSAssert(![x isZero], @"Attempted to divide by zero");
    
	if (self == x) {
		// x / x = 1 r 0
		limbs = [[MPLimbVector alloc] initWithSize:1];
		[limbs setItem:0 toValue:1];
        
		return [[MPMagnitude alloc] initWithLimbValue:0];
	} else if ([x numberOfLimbs] == 1) {
		// If the divisor consists of a single digit we have the fast algorithm
		MPLimb rem = [self divideLimbValue:[x limb:0]];
        
		return [[MPMagnitude alloc] initWithLimbValue:rem];
	} else if ([self isLessThan:x]) {
		// If the divisor is greater than the dividend then we simply have this as the remainder
		MPMagnitude *remainder = [[MPMagnitude alloc] initWithLimbs:limbs];
        limbs = [[MPLimbVector alloc] initWithSize:1];
		return remainder;
	}
	else
	{
		// We'll have to perform long division
		return [self longDivide:x];
	}
}

- (MPMagnitude *)longDivide:(MPMagnitude *)divisor {
    // The following method is the one specified by Knuth in The Art of Computer Programming, Volume 2.
    
	// Obtain the factor to normalize with
	MPDLimb v1 = [divisor limb:[divisor numberOfLimbs] - 1];
	MPLimb norm = BASE / (v1 + 1);
    
	// The normalized dividend with an added digit (u_0)
	MPMagnitude *normalizedDividend = [[MPMagnitude alloc] initWithLimbs:limbs];
    [normalizedDividend->limbs growToSize:[normalizedDividend numberOfLimbs] + 1];
    [normalizedDividend multiplyLimbValue:norm];
    
	// The normalized divisor
	MPMagnitude *normalizedDivisor = [divisor copy];
    [normalizedDividend multiplyLimbValue:norm];
    
	// D2 through 7
	MPMagnitude *remainder = [self normalizedLongDivide:normalizedDividend by:normalizedDivisor];
    
	// D8: Unnormalize
    [remainder divideLimbValue:norm];
    
    [limbs discardLeadingZeros];
    [remainder->limbs discardLeadingZeros];
    
	return remainder;
}

- (MPLimb)estimateQuotientDigitOf:(MPMagnitude *)u dividedBy:(MPMagnitude *)v {
	const MPDLimb u0 = [u limb:[u numberOfLimbs] - 1];
    const MPDLimb u1 = [u limb:[u numberOfLimbs] - 2];
    const MPDLimb u2 = [u limb:[u numberOfLimbs] - 3];
    const MPDLimb v1 = [v limb:[v numberOfLimbs] - 1];
    const MPDLimb v2 = [v limb:[v numberOfLimbs] - 2];
    
	// The guess at the quotient digit
	MPLimb q;
	MPDLimb r;
    
	// Calculate our guess
	if (u0 == v1)
	{
		q = BASE - 1;
		r = u0 + u1;
	}
	else
	{
		q = (MPLimb)((u0 * BASE + u1) / v1);
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

- (MPMagnitude *)normalizedLongDivide:(MPMagnitude *)dividend by:(MPMagnitude *)divisor {
	const NSUInteger n = [divisor numberOfLimbs];
	// len(u) = m + n + 1 => m = len(u) - n - 1
	const NSUInteger m = [dividend numberOfLimbs] - n - 1;
    
	MPMagnitude *remainder = [[MPMagnitude alloc] initWithLimbs:[[MPLimbVector alloc] initWithSize:[divisor numberOfLimbs] + 1]];
    
	// Copy digits u_0 ... u_n-1 into the remainder
	// u_n will be copied when the loop starts
	for (NSUInteger i = 0; i < n; ++i)
	{
		const NSUInteger offset = [dividend numberOfLimbs] - n;
        [remainder->limbs setItem:i toValue:[dividend limb:offset + i]];
	}
    
	// We'll create this alias to make things clear
	MPMagnitude *quotient = self;
    
	quotient->limbs = [[MPLimbVector alloc] initWithSize:m + 1];
    
	for (NSUInteger j = 0; j <= m; ++j)
	{
		// Bring the next digit down
		[remainder shiftLeft];
		NSUInteger nextDigitIndex = [dividend numberOfLimbs] - n - j - 1;
		MPLimb nextDigit = [dividend limb:nextDigitIndex];
        [remainder->limbs setItem:0 toValue:nextDigit];
        
		// D3: Estimate the quotient
		MPLimb q = [self estimateQuotientDigitOf:remainder dividedBy:divisor];
        
		// D4: Long divide subtract and multiply
		MPMagnitude *subtracted = [divisor copy];
        [subtracted multiplyLimbValue:q];
        
		if ([remainder isLessThan:subtracted])
		{
			// D6: Add back
			--q;
            
			// Add the divisor
            [remainder add:divisor];
            
			// Truncate the added digit
            [remainder->limbs shrinkToSize:[remainder numberOfLimbs] - 1];
		}
		else
		{
			// Continue D4
            [remainder subtract:subtracted];
		}
        
		// D5: Set quotient digit
        [quotient shiftLeft];
        [quotient->limbs setItem:0 toValue:q];
	}
    
	return remainder;
}

- (void)addLimbValue:(MPLimb)x {
    MPDLimb carry = x;
    
	NSUInteger i = 0;
	
	while (carry > 0)
	{
        [limbs growToSize:i + 1];
        
        MPDLimb result = carry + (MPDLimb)[self limb:i];
        [limbs setItem:i toValue:LOW(result)];
		carry = HIGH(result);
        
		assert(carry <= 1);
        
		++i;
	}
}

- (void)subtractLimbValue:(MPLimb)x {
    NSAssert(![self isLessThanLimbValue:x], @"Attempted to subtract a limb value which was greater than the minuend");
	
	MPDLimb value = [self limb:0];
	MPDLimb xValue = x;
    
	// We may need to borrow from the next digit
	if (xValue > value)
	{
		[self borrowForIndex:0];
		value += BASE;
	}
    
	NSAssert(HIGH(value - xValue) == 0, @"Borrowing took place when it was not supposed to; the result is larger than a limb can hold");
    [limbs setItem:0 toValue:(MPLimb)LOW(value - xValue)];
    
    [limbs discardLeadingZeros];
}

- (void)multiplyLimbValue:(MPLimb)x {
    // Store this instances' value
	MPMagnitude *y = [[MPMagnitude alloc] initWithLimbs:limbs];
    
	// Zero out our current integer -- we'll be adding everything to it
	limbs = [[MPLimbVector alloc] initWithSize:[y numberOfLimbs]];
    
	// Multiply each digit with x and store the result in this instance
	for (NSUInteger i = 0; i < [y numberOfLimbs]; ++i)
	{
		MPDLimb result = (MPDLimb)[y limb:i] * x;
        
        [self addDLimb:result atIndex:i];
	}
}

- (MPLimb)divideLimbValue:(MPLimb)x {
    MPDLimb carry = 0;
	MPDLimb temp;
    
	const MPMagnitude *copy = [[MPMagnitude alloc] initWithLimbs:limbs];
    limbs = [[MPLimbVector alloc] initWithSize:[copy numberOfLimbs]];
    
	const NSUInteger last = [copy numberOfLimbs] - 1;
    
	for (NSUInteger i = 0; i < [copy numberOfLimbs]; ++i)
	{
		temp = carry * BASE + [copy limb:last - i];
        
        [limbs setItem:last - i toValue:(MPLimb)(temp / x)];
		carry = temp % x;
	}
    
    [limbs discardLeadingZeros];
    
	return (MPLimb)carry;
}


- (BOOL)equals:(MPMagnitude *)x {
    NSUInteger highest = [limbs highestNonZeroIndex];
	NSUInteger xHighest = [x->limbs highestNonZeroIndex];
    
    // Ensure they have the same length
	if (highest != xHighest)
	{
		return false;
	}
    
	for (NSUInteger i = 0; i < highest; ++i)
	{
		if ([self limb:i] != [x limb:i])
		{
			return false;
		}
	}
    
	return true;
}

- (BOOL)isLessThan:(MPMagnitude *)x {
    NSUInteger highest = [limbs highestNonZeroIndex];
    NSUInteger xHighest = [x->limbs highestNonZeroIndex];
    
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
        
		for (NSUInteger i = highest; i > 0; --i)
		{
			MPLimb value = [self limb:i];
			MPLimb xValue = [x limb:i];
            
			if (value != xValue)
			{
				// Test whether the other integer has a higher value for its digit
				return value < xValue;
			}
		}
	}
    
	return [self limb:0] < [x limb:0];
}

- (BOOL)isLessThanLimbValue:(MPLimb)x {
    if ([limbs highestNonZeroIndex] != 0) {
        return false;
    } else {
        return [self limb:0] < x;
    }
}

- (BOOL)isZero {
	for (NSUInteger i = 0; i < [self numberOfLimbs]; ++i)
	{
		if ([self limb:i] != 0)
		{
			return false;
		}
	}
    
	return true;
}

- (void)shiftLeft {
	// A single shift left can be seen as multiplication by UINT32_MAX
	[self multiplyLimbValue:BASE];
}

- (NSString *)internalRepresentation {
    NSMutableString *desc = [NSMutableString string];
    for (NSUInteger i = 0; i < [self numberOfLimbs]; ++i) {
        [desc appendFormat:@"\t[%lu]\t%u\n", i, [self limb:i]];
    }
    return desc;
}

@end
