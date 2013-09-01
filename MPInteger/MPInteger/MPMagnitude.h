//
//  MPMagnitude.h
//  MPInteger
//
//  Created by Stuart Johnston on 1/09/13.
//  Copyright (c) 2013 Stuart Johnston. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MPLimbVector.h"

@interface MPMagnitude : NSObject {
    MPLimbVector *limbs;
}

- (id)initWithLimbValue:(MPLimb)value;

- (MPMagnitude *)copy;

- (void)add:(MPMagnitude *)x;
- (void)subtract:(MPMagnitude *)x;
- (void)multiply:(MPMagnitude *)x;
- (MPMagnitude *)divide:(MPMagnitude *)x;

- (void)addLimbValue:(MPLimb)x;
- (void)subtractLimbValue:(MPLimb)x;
- (void)multiplyLimbValue:(MPLimb)x;
- (MPLimb)divideLimbValue:(MPLimb)x;


- (BOOL)equals:(MPMagnitude *)x;
- (BOOL)isLessThan:(MPMagnitude *)x;
- (BOOL)isLessThanLimbValue:(MPLimb)x;
- (BOOL)isZero;

- (NSString *)internalRepresentation;

@end
