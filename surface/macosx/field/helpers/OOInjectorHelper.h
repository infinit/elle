//
//  OOHelpers.h
//  Infinit
//
//  Created by Charles Guillot - Infinit on 3/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

extern NSString *OOHelperInstalled;

@interface OOInjectorHelper : NSObject

+ (BOOL)launchFinderHelperTools:(BOOL)blessHelper;
+ (BOOL)blessHelperWithLabel:(NSString *)label error:(NSError **)error;

@end
