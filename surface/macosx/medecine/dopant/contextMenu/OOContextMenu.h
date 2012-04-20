//
//  OOContextMenu.h
//  Dopant
//
//  Created by Charles Guillot - Infinit on 3/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OOSwizzle.h"

/**
 This class add the infinit submenu adhoc context menu
 */
@interface OOContextMenu : NSObject

/**
 Get the singleton instance
 @returns the instance
 */
+ (OOContextMenu*) instance;

@end
