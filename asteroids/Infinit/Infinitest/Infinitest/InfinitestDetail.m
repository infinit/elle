//
//  InfinitestDetail.m
//  Infinitest
//
//  Created by Simon Peccaud on 3/20/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "InfinitestDetail.h"

@implementation InfinitestDetail

@synthesize description = _description;

- (id)initWithTitle:(NSString*)description {
    if ((self = [super init])) {
        self.description = description;
    }
    return self;
}

@end
