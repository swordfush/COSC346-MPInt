//
//  MPLimbVector.h
//  MPInteger
//
//  Created by Stuart Johnston on 1/09/13.
//  Copyright (c) 2013 Stuart Johnston. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef uint32_t MPLimb;
typedef uint64_t MPDLimb;

#define MPLIMB_MAX UINT32_MAX

/**
 * A vector to hold limbs.
 */
@interface MPLimbVector : NSObject {
    MPLimb *limbs;
    NSUInteger size;
}

/**
 * Initializes a new vector with initialSize limbs all set to zero.
 */
- (id)initWithSize:(NSUInteger)initialSize;

/**
 * Creates a copy of this instance.
 */
- (MPLimbVector *)copy;

/**
 * Grows this instance to contain newSize limbs, as long as newSize is 
 * greater than its current size.
 */
- (void)growToSize:(NSUInteger)newSize;

/**
 * Discards all leading zero elements.
 */
- (void)discardLeadingZeros;

/**
 * Shrinks to contain newSize limbs, as long as newSize is less than the
 * current size.
 */
- (void)shrinkToSize:(NSUInteger)newSize;

/**
 * Returns the index of the highest non-zero limb. If all limbs are zero then
 * 0 is returned as the index.
 */
- (NSUInteger)highestNonZeroIndex;

/**
 * Obtains the limb at the index specified.
 */
- (MPLimb)item:(NSUInteger)index;

/**
 * Sets the limb at the index specified.
 */
- (void)setItem:(NSUInteger)index toValue:(MPLimb)value;

/**
 * Returns the number of limbs currently in this vector.
 */
- (NSUInteger)size;

@end
