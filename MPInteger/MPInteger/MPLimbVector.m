//
//  MPLimbVector.m
//  MPInteger
//
//  Created by Stuart Johnston on 1/09/13.
//  Copyright (c) 2013 Stuart Johnston. All rights reserved.
//

#import "MPLimbVector.h"

@implementation MPLimbVector

- (id)initWithSize:(NSUInteger)initialSize {
    self = [super init];
    if (self) {
        limbs = nil;
        size = 0;
        [self setSize:initialSize];
    }
    
    return self;
}

- (void)dealloc {
    free(limbs);
}

- (MPLimbVector *)copy {
    MPLimbVector *copy = [[MPLimbVector alloc] initWithSize:[self size]];
    
    for (NSUInteger i = 0; i < [self size]; ++i) {
        [copy setItem:i toValue:[self item:i]];
    }
    
    return copy;
}

- (void)setSize:(NSUInteger)newSize {
    limbs = realloc(limbs, newSize * sizeof(*limbs));
    
    if (limbs == nil) {
        // The allocation failed!
        size = 0;
    } else {
        // We'll zero out any added entries
        for (NSUInteger i = size; i < newSize; ++i) {
            limbs[i] = 0;
        }
        
        size = newSize;
    }
}

- (void)growToSize:(NSUInteger)newSize {
    if (newSize > [self size]) {
        [self setSize:newSize];
    }
}

- (void)discardLeadingZeros {
    [self setSize:[self highestNonZeroIndex] + 1];
}

- (void)shrinkToSize:(NSUInteger)newSize {
    if (newSize < [self size]) {
        if (newSize == 0) {
            [self setSize:1];
        } else {
            [self setSize:newSize];
        }
    }
}

- (NSUInteger)highestNonZeroIndex {
    for (NSUInteger i = [self size] - 1; i > 0; --i) {
        if ([self item:i] != 0) {
            return i;
        }
    }
    
    return 0;
}

- (MPLimb)item:(NSUInteger)index {
    NSAssert(index < [self size], @"The index provided is outside the bounds of the vector.");
    return limbs[index];
}

- (void)setItem:(NSUInteger)index toValue:(MPLimb)value {
    NSAssert(index < [self size], @"The index provided is outside the bounds of the vector.");
    limbs[index] = value;
}

- (NSUInteger)size {
    return size;
}


@end
