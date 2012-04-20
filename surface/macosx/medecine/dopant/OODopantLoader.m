//
//  OODopantLoader.m
//  Dopant
//
//  Created by Charles Guillot - Infinit on 2/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OODopantLoader.h"
#import "OOContextMenu.h"
#import "OOIconOverlay.h"
#import "OOFavorites.h"

@implementation OODopantLoader

+ (void) load
{
    [OOIconOverlay instance];
    [OOFavorites instance];
    [OOContextMenu instance];
}

@end
