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

@synthesize sourceDir;
@synthesize sourceFinderBundleFullPath;
@synthesize sourceMachBundleFullPath;

- (BOOL)injectWithAppPath:(NSString *)arg1
{
    return [self injectWithAppPath:arg1 forceInstall:NO];
}

- (BOOL)injectWithAppPath:(NSString *)arg1 forceInstall:(BOOL)arg2
{
    
    self.sourceDir = [arg1 stringByAppendingString:sourceDirRelativePath];
    self.sourceFinderBundleFullPath = [sourceDir stringByAppendingString:finderBundleName];
    self.sourceMachBundleFullPath = [sourceDir stringByAppendingString:machBundleName];
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:destinationDirFullPath] && arg2)
    {
        if (![self createDirectory])
            return NO;
    }
    if (![[NSFileManager defaultManager] fileExistsAtPath:destinationFinderBundleFullPath] && arg2)
    {
        if (![self installFinderBundle])
            return NO;
    }
    if (![[NSFileManager defaultManager] fileExistsAtPath:destinationMachBundleFullPath] && arg2)
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
    
	syslog(LOG_NOTICE, "Injecting to Finder bundle at path : %s \n", [destinationFinderBundleFullPath UTF8String]);
	mach_error_t err = mach_inject_bundle_pid([destinationFinderBundleFullPath fileSystemRepresentation],
                                              [destinationMachBundleFullPath fileSystemRepresentation],
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
        if ([[NSFileManager defaultManager] createDirectoryAtPath:destinationDirFullPath withIntermediateDirectories:YES attributes:nil error:nil])
            syslog(LOG_NOTICE, "PrivilegiedDirectory created");
        return [[NSFileManager defaultManager] fileExistsAtPath:destinationDirFullPath];
    }
    @catch (NSException *exception) {
        syslog(LOG_NOTICE, "Exception: %s", [[exception name] UTF8String]);
        return NO;
    }
}
- (BOOL)installFinderBundle
{
    @try {
        if ( [[NSFileManager defaultManager] isReadableFileAtPath:self.sourceFinderBundleFullPath] )
        {
            if ([[NSFileManager defaultManager] fileExistsAtPath:destinationFinderBundleFullPath])
            {
                if ([[NSFileManager defaultManager] removeItemAtPath:destinationFinderBundleFullPath error:nil])
                {
                    syslog(LOG_NOTICE, "Finder bundle removed");
                }
                else
                {
                    return NO;
                }
            }
            if ([[NSFileManager defaultManager] copyItemAtPath:self.sourceFinderBundleFullPath toPath:destinationFinderBundleFullPath error:nil])
                syslog(LOG_NOTICE, "Finder bundle copied");
        }
        return [[NSFileManager defaultManager] fileExistsAtPath:destinationFinderBundleFullPath];
    }
    @catch (NSException *exception) {
        syslog(LOG_NOTICE, "Exception: %s", [[exception name] UTF8String]);
        return NO;
    }
}
- (BOOL)installMachBundle
{
    @try {
        if ( [[NSFileManager defaultManager] isReadableFileAtPath:self.sourceMachBundleFullPath] )
        {
            if ([[NSFileManager defaultManager] fileExistsAtPath:destinationMachBundleFullPath])
            {
                if ([[NSFileManager defaultManager] removeItemAtPath:destinationMachBundleFullPath error:nil])
                {
                    syslog(LOG_NOTICE, "Mach bundle removed");
                }
                else
                {
                    return NO;
                }
            }
            if ([[NSFileManager defaultManager] copyItemAtPath:self.sourceMachBundleFullPath toPath:destinationMachBundleFullPath error:nil])
                syslog(LOG_NOTICE, "Mach bundle copied");
        }
        return [[NSFileManager defaultManager] fileExistsAtPath:destinationMachBundleFullPath];
    }
    @catch (NSException *exception) {
        syslog(LOG_NOTICE, "Exception: %s", [[exception name] UTF8String]);
        return NO;
    }
    
    
}
@end
