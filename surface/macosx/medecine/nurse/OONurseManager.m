//
//  OONurseManager.m
//  medecine
//
//  Created by Charles Guillot on 21/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OONurseManager.h"
#import "OONurseFuser.h"

@implementation OONurseManager

- (BOOL)manage:(NSString *)arg1
{
    OONurseFuser *nurseFuse = [[OONurseFuser alloc] init];
    BOOL fuseInstalled = [nurseFuse installFuseWithAppPath:arg1];
    return fuseInstalled;
}


@end
