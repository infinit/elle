//
//  OOFavorites.h
//  Dopant
//
//  Created by Charles Guillot - Infinit on 3/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OOSwizzle.h"
#import "CDStructures.h"

/**
 This class loads favorite to the favorite sidebar
 */
@interface OOFavorites : NSObject
{
    /**
     The sidebar image
     */
    NSImage                 *sidebarImage;
    /**
     The mountPath
     */
    NSString                *infinitMountPath;
}

@property(retain) NSImage   *sideBarImage;
@property(retain) NSString  *infinitMountPath;

/**
 Get the singleton instance
 @returns the instance
 */
+ (OOFavorites*) instance;

/**
 Add the folder to the favorite sidebar
 */
- (void) addToFavorite;

@end
