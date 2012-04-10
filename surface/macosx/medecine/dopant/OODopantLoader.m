//
//  IHFBMain.m
//  InfinitHelperTools
//
//  Created by Charles Guillot - Infinit on 2/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OODopantLoader.h"
#import "OOContextMenu.h"
#import "OOIconOverlay.h"
#import "OOAddToFavorites.h"

@implementation OODopantLoader

+ (void) load
{
    [OOIconOverlay instance];
    [OOAddToFavorites instance];
    [OOContextMenu instance];
}

@end
