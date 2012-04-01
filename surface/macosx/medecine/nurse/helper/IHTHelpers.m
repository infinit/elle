//
//  IHTHelpers.m
//  InfinitHelperTools
//
//  Created by Charles Guillot - Infinit on 2/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "IHTHelpers.h"
#import <AppKit/AppKit.h>

@implementation IHTHelpers
- (pid_t) getFinderPid
{
    NSLog(@"Start Finding processes...");
    NSArray *finders = [NSRunningApplication runningApplicationsWithBundleIdentifier:@"com.apple.finder"];
    if (finders == nil || [finders count] <= 0)
    {
        return -1; // Empty finders (there is no finders)
    }
    return [[finders objectAtIndex:0] processIdentifier]; // Return the pid of the first Finder
}
@end
