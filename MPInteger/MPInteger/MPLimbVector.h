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

@interface MPLimbVector : NSObject {
    MPLimb *limbs;
    NSUInteger size;
}

- (id)initWithSize:(NSUInteger)initialSize;

- (MPLimbVector *)copy;

- (void)growToSize:(NSUInteger)newSize;
- (void)discardLeadingZeros;
- (void)shrinkToSize:(NSUInteger)newSize;

- (NSUInteger)highestNonZeroIndex;

- (MPLimb)item:(NSUInteger)index;
- (void)setItem:(NSUInteger)index toValue:(MPLimb)value;

- (NSUInteger)size;

@end
