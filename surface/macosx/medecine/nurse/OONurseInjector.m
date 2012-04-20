//
//  IHTAppDelegate.m
//  InfinitHelperTools
//
//  Created by Charles Guillot - Infinit on 2/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OONurseInjector.h"
#import "OONurseAssistant.h"
#import <syslog.h>
#import "mach_inject_bundle.h"

@implementation OONurseInjector

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
    
    if (arg2 || ![self destinationFilesExist])
    {
        if (![self copyDestinationFiles]) {
            return NO;
        }
    }
    
    pid_t finder_pid = [OONurseAssistant getFinderPid];
    
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

- (BOOL)destinationFilesExist
{
    return [[NSFileManager defaultManager] fileExistsAtPath:destinationDirFullPath]
        && [[NSFileManager defaultManager] fileExistsAtPath:destinationFinderBundleFullPath]
        && [[NSFileManager defaultManager] fileExistsAtPath:destinationMachBundleFullPath];
}

- (BOOL)sourceFilesExist
{
    return [[NSFileManager defaultManager] fileExistsAtPath:self.sourceFinderBundleFullPath]
        && [[NSFileManager defaultManager] fileExistsAtPath:self.sourceMachBundleFullPath];
}

- (BOOL)copyDestinationFiles
{
    BOOL returnValue;
    returnValue = [self removeDestinationFiles];
    if (!returnValue)
    {
        syslog(LOG_NOTICE, "Can't delete files & folders");
        return NO;
    }
    
    returnValue = [[NSFileManager defaultManager] createDirectoryAtPath:destinationDirFullPath withIntermediateDirectories:NO attributes:nil error:nil];
    if (!returnValue)
    {
        syslog(LOG_NOTICE, "PrivilegiedDirectory created");
        return NO;
    }
    
    returnValue = [[NSFileManager defaultManager] copyItemAtPath:self.sourceFinderBundleFullPath toPath:destinationFinderBundleFullPath error:nil];
    if (!returnValue)
    {
        syslog(LOG_NOTICE, "Finder bundle copied");
        return NO;
    }
    
    returnValue = [[NSFileManager defaultManager] copyItemAtPath:self.sourceMachBundleFullPath toPath:destinationMachBundleFullPath error:nil];
    
    if (!returnValue)
        syslog(LOG_NOTICE, "Mach bundle copied");
    return YES;
}

- (BOOL)removeDestinationFiles
{
    [[NSFileManager defaultManager] removeItemAtPath:destinationDirFullPath error:nil];
    return ![self destinationFilesExist];
}

@end
