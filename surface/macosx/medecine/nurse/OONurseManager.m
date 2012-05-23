//
//  OONurseManager.m
//  medecine
//
//  Created by Charles Guillot on 21/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OONurseManager.h"

@implementation OONurseManager

- (BOOL)manage:(NSString *)arg1
{
    //TODO
    //OONurseInjector *nurseInj = [[OONurseInjector alloc] init];
    OONurseFuser *nurseFuse = [[OONurseFuser alloc] init];
    
    return /*[nurseInj injectWithAppPath:arg1] && */[nurseFuse installFuseWithAppPath:arg1];
}


@end
