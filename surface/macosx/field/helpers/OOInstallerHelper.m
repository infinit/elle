//
//  OOInstallerHelper.m
//  Infinit
//
//  Created by Charles Guillot - Infinit on 3/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOInstallerHelper.h"

@implementation OOInstallerHelper

+ (BOOL)checkPreferencePanel
{
    @try {
        if ( [[NSFileManager defaultManager] isReadableFileAtPath:prefPaneSource] )
        {
            if ([[NSFileManager defaultManager] fileExistsAtPath:prefPaneDestination])
            {
                if ([[NSFileManager defaultManager] removeItemAtPath:prefPaneDestination error:nil])
                {
                    NSLog(@"Mach bundle removed");
                }
                else
                {
                    return NO;
                }
            }
            [[NSFileManager defaultManager] copyItemAtPath:prefPaneSource toPath:prefPaneDestination error:nil];
            NSLog(@"Mach bundle copied");
        }
        return [[NSFileManager defaultManager] fileExistsAtPath:prefPaneDestination];
    }
    @catch (NSException *exception) {
        NSLog(@"Exception: %@", exception);
        return NO;
    }
}


+ (BOOL)installPreferencePanel
{
    //[[NSBundle mainBundle] 
    return YES;
}

@end
