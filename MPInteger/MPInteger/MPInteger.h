//
//  MPInteger.h
//  MPInteger
//
//  Created by Stuart Johnston on 1/09/13.
//  Copyright (c) 2013 Stuart Johnston. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "MPMagnitude.h"

@interface MPInteger : NSObject {
    MPMagnitude *magnitude;
    BOOL isNegative;
}


- (id)initWithString:(NSString *)str;
- (NSString *)description;

- (MPInteger *)copy;

- (MPInteger *)add:(MPInteger *)x;
- (MPInteger *)subtract:(MPInteger *)x;
- (MPInteger *)multiply:(MPInteger *)x;
- (MPInteger *)divideBy:(MPInteger *)x;
- (MPInteger *)modulus:(MPInteger *)x;

- (BOOL)isLessThan:(MPInteger *)x;


@end
