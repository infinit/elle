//
//  IHTAppDelegate.m
//  InfinitHelperTools
//
//  Created by Charles Guillot - Infinit on 2/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "IHTInjector.h"
#import "IHTHelpers.h"
#import <syslog.h>
#import "mach_inject_bundle.h"

@implementation IHTInjector

- (BOOL)inject
{
    return [self inject:NO];
}

- (BOOL)inject:(BOOL)forceInstallBundle
{
    if (![[NSFileManager defaultManager] fileExistsAtPath:destinationDirectory] && forceInstallBundle)
    {
        if (![self createDirectory])
            return NO;
    }
    if (![[NSFileManager defaultManager] fileExistsAtPath:finderBundleDestination] && forceInstallBundle)
    {
        if (![self installFinderBundle])
            return NO;
    }
    if (![[NSFileManager defaultManager] fileExistsAtPath:machBundleDestination] && forceInstallBundle)
    {
        if (![self installMachBundle])
            return NO;
    }
    IHTHelpers* helper = [[IHTHelpers alloc] init];
    pid_t finder_pid = [helper getFinderPid];
    if (finder_pid < 0) 
    {
        NSLog(@"Finder can't be find : %@ \n", finder_pid);
        return NO; // Can't get Finder pid
    }
    
	syslog(LOG_NOTICE, "Injecting to Finder bundle at path : %s \n", [finderBundleDestination UTF8String]);
	mach_error_t err = mach_inject_bundle_pid([finderBundleDestination fileSystemRepresentation],
                                              [machBundleDestination fileSystemRepresentation],
                                              finder_pid );
    
	if (!err)
    {
        syslog(LOG_NOTICE, "Bundle injected\n");
    }
    else
    {
        syslog(LOG_NOTICE, "Bundle injection failed\n");
        return NO;
    }
    return YES;
}
- (BOOL)createDirectory
{
    @try {
        if ([[NSFileManager defaultManager] createDirectoryAtPath:destinationDirectory withIntermediateDirectories:YES attributes:nil error:nil])
            syslog(LOG_NOTICE, "PrivilegiedDirectory created");
        return [[NSFileManager defaultManager] fileExistsAtPath:destinationDirectory];
    }
    @catch (NSException *exception) {
        syslog(LOG_NOTICE, "Exception: %s", [[exception name] UTF8String]);
        return NO;
    }
}
- (BOOL)installFinderBundle
{
    @try {
        if ( [[NSFileManager defaultManager] isReadableFileAtPath:finderBundleSource] )
        {
            if ([[NSFileManager defaultManager] fileExistsAtPath:finderBundleDestination])
            {
                if ([[NSFileManager defaultManager] removeItemAtPath:finderBundleDestination error:nil])
                {
                    syslog(LOG_NOTICE, "Finder bundle removed");
                }
                else
                {
                    return NO;
                }
            }
            if ([[NSFileManager defaultManager] copyItemAtPath:finderBundleSource toPath:finderBundleDestination error:nil])
                syslog(LOG_NOTICE, "Finder bundle copied");
        }
        return [[NSFileManager defaultManager] fileExistsAtPath:finderBundleDestination];
    }
    @catch (NSException *exception) {
        syslog(LOG_NOTICE, "Exception: %s", [[exception name] UTF8String]);
        return NO;
    }
}
- (BOOL)installMachBundle
{
    @try {
        if ( [[NSFileManager defaultManager] isReadableFileAtPath:machBundleSource] )
        {
            if ([[NSFileManager defaultManager] fileExistsAtPath:machBundleDestination])
            {
                if ([[NSFileManager defaultManager] removeItemAtPath:machBundleDestination error:nil])
                {
                    syslog(LOG_NOTICE, "Mach bundle removed");
                }
                else
                {
                    return NO;
                }
            }
            if ([[NSFileManager defaultManager] copyItemAtPath:machBundleSource toPath:machBundleDestination error:nil])
                syslog(LOG_NOTICE, "Mach bundle copied");
        }
        return [[NSFileManager defaultManager] fileExistsAtPath:machBundleDestination];
    }
    @catch (NSException *exception) {
        syslog(LOG_NOTICE, "Exception: %s", [[exception name] UTF8String]);
        return NO;
    }
    
    
}
@end
