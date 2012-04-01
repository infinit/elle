//
//  IHFBMain.m
//  InfinitHelperTools
//
//  Created by Charles Guillot - Infinit on 2/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FIMain.h"
#import "IHFMenuNeedsUpdate.h"
#import "FIIconOverlay.h"
#import "TestClass.h"
#import "FIAddInfinitToFavorite.h"

@implementation IHFBMain

+ (void) load
{
    [FIIconOverlay instance];
    [FIAddInfinitToFavorite instance];
    [TestClass instance];
    [IHFMenuNeedsUpdate instance];
}

@end
