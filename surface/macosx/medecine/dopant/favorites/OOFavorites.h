//
//  AddInfinitToFavorite.h
//  HelperTools
//
//  Created by Charles Guillot - Infinit on 3/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OOSwizzle.h"
#import "CDStructures.h"

@interface OOFavorites : NSObject
{
    NSImage                 *sidebarImage;
    NSString                *infinitMountPath;
}

@property(retain) NSImage   *sideBarImage;
@property(retain) NSString  *infinitMountPath;

+ (OOFavorites*) instance;

- (void) addToFavorite;

@end
