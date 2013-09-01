//
//  MPInteger.m
//  MPInteger
//
//  Created by Stuart Johnston on 1/09/13.
//  Copyright (c) 2013 Stuart Johnston. All rights reserved.
//

#import "MPInteger.h"

@implementation MPInteger

- (id)initWithMagnitude:(MPMagnitude *)mag andSign:(BOOL)isSigned {
    self = [super init];
    if (self) {
        magnitude = mag;
        isNegative = isSigned;
    }
    return self;
}

- (id)initWithString:(NSString *)str {
    self = [self initWithMagnitude:[[MPMagnitude alloc] initWithLimbValue:0] andSign:false];
    if (self) {
        NSUInteger i = 0;
        
        if ([str characterAtIndex:i] == '-') {
            ++i;
            isNegative = true;
        } else if ([str characterAtIndex:i] == '+') {
            ++i;
        }
        
        for (; i < [str length]; ++i) {
            [magnitude multiplyLimbValue:10];
            
            unichar chr = [str characterAtIndex:i];
            NSAssert(chr >= '0' && chr <= '9', @"The input string contains invalid characters");
            
            [magnitude addLimbValue:chr - '0'];
        }
    }
    
	return self;
}

- (NSString *)reverseString:(NSString *)str {
    NSMutableString *reversed = [NSMutableString string];
    const NSUInteger lastIndex = [str length] - 1;
    for (NSUInteger offset = 0; offset < [str length]; ++offset) {
        [reversed appendString:[str substringWithRange:NSMakeRange(lastIndex - offset, 1)]];
    }
    return reversed;
}

- (NSString *)description {
    if ([magnitude isZero]) {
		return @"0";
	}
    
	MPMagnitude *copy = [magnitude copy];
	NSMutableString *desc = [NSMutableString string];
    
	while (![copy isZero]) {
		MPLimb remainder = [copy divideLimbValue:10];
        [desc appendFormat:@"%c", ('0' + remainder)];
	}
    
    if (isNegative) {
        [desc appendFormat:@"-"];
    }
    
    return [self reverseString:desc];
}

- (MPInteger *)copy {
    return [[MPInteger alloc] initWithMagnitude:[magnitude copy] andSign:isNegative];
}

- (MPInteger *)add:(MPInteger *)x {
    MPInteger *result;
    
	if (isNegative == x->isNegative) {
		// Both integers have the same sign
		// x + y = x + y
		// -x + -y = -(x + y)
		// So we copy (including the sign) and add the magnitudes
        result = [self copy];
        [result->magnitude add:x->magnitude];
		return result;
	} else {
		// Both integers have different signs, so we need to perform subtraction
		// a + (-b)
		const MPInteger *a;
		const MPInteger *b;
        
		if (isNegative) {
			a = x;
			b = self;
		} else {
			a = self;
			b = x;
		}
        
		// If b > a then we'll run into an issue, use a - b = -(b - a)
		if ([a->magnitude isLessThan:b->magnitude]) {
			result = [b copy];
			[result->magnitude subtract:a->magnitude];
			// b's sign is copied
		} else {
			result = [a copy];
            [result->magnitude subtract:b->magnitude];
			// a's sign is copied
		}
	}
    
	return result;
}

- (MPInteger *)subtract:(MPInteger *)x {
    // We'll use the identity x - y = x + -y = -(-x + y)
    
	// To prevent allocating a scratch instance we'll temporarily switch the sign of x
	x->isNegative = !x->isNegative;
    
	// Perform addition with -x
	MPInteger *result = [self add:x];
    
	// Switch it back...
	x->isNegative = !x->isNegative;
    
	return result;
}

- (MPInteger *)multiply:(MPInteger *)x {
    MPInteger *result = [self copy];
    [result->magnitude multiply:x->magnitude];
    
	result->isNegative = isNegative ^ x->isNegative;
    
	return result;
}

- (MPInteger *)divideBy:(MPInteger *)x {
    MPInteger *result = [self copy];
	MPMagnitude *rem = [result->magnitude divide:x->magnitude];
    
	result->isNegative = isNegative ^ x->isNegative;
    
	// Handle the result of floored division where the result is negative
	if (result->isNegative && ![rem isZero]) {
        [result->magnitude addLimbValue:1];
	}

	return result;
}

- (MPInteger *)modulus:(MPInteger *)x {
    BOOL sign;
	MPMagnitude *divisor = [magnitude copy];
	MPMagnitude *mod = [divisor divide:x->magnitude];
    
	if ([mod isZero]) {
		// The remainder is zero; prevent signed zero
		sign = false;
	} else if (isNegative != x->isNegative) {
		// The integers have opposite signs
		// We can work out new magnitude as x - (this % x)
		MPMagnitude *oldMagnitude = mod;
		mod = [x->magnitude copy];
        [mod subtract:oldMagnitude];
        
		// If x was signed then the result will be
		sign = x->isNegative;
	}
	else
	{
		// The integers have the same sign. If taking the modulus of two
		// negative numbers then the result is negative
		sign = isNegative;
	}
    
	return [[MPInteger alloc] initWithMagnitude:mod andSign:sign];
}

- (BOOL)isLessThan:(MPInteger *)x {
    if (isNegative == x->isNegative)
	{
		if (isNegative)
		{
			// Both are negative
			// If the absolute value of x is less than this then this is
			// 'more negative' and therefore less than x
            return [x->magnitude isLessThan:magnitude];
		}
		else
		{
			// Both are positive -- simply use unsigned check
            return [magnitude isLessThan:x->magnitude];
		}
	}
	else
	{
		// Both have different signs, so if this is negative then it is smaller
		return isNegative;
	}
}


@end