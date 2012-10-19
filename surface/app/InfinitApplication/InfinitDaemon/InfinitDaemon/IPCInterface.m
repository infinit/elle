//
//  IPCInterface.m
//  InfinitDaemon
//
//  Created by infinit on 10/18/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IPCInterface.h"
#import "libmach/mach_inject_bundle.h"
#import <syslog.h>

@implementation IPCInterface

+ (IPCInterface*)getInstance
{
    static IPCInterface* instance = nil;
    if (instance == nil)
        instance = [[IPCInterface alloc] init];
    return instance;
}

- (id)init
{
    self = [super init];
    if (self)
        self.should_keep_running = YES;
    return self;
}


+ (pid_t) _getFinderPid
{
    NSArray *finders = [NSRunningApplication runningApplicationsWithBundleIdentifier:@"com.apple.finder"];
    if (finders == nil || [finders count] <= 0)
    {
        return -1; // Empty finders (there is no finders)
    }
    return [[finders objectAtIndex:0] processIdentifier]; // Return the pid of the first Finder
}


- (void)injectBundle:(NSString*)source_bundle_path stubBundlePath:(NSString*)stub_bundle_path
{
    pid_t process_id = [IPCInterface _getFinderPid];
    if (!process_id)
    {
        syslog(LOG_ERR, "Couldn't find the finder");
        return;
    }
	syslog(LOG_NOTICE, "injecting bundle %s into finder pid %d", [source_bundle_path UTF8String], process_id);
	
	mach_error_t err = mach_inject_bundle_pid([source_bundle_path fileSystemRepresentation],
                                              [stub_bundle_path fileSystemRepresentation], process_id);
    switch (err)
    {
        case 0:
            syslog(LOG_NOTICE, "Successfully injected bundle");
            break;
        case err_mach_inject_bundle_couldnt_load_framework_bundle:
            syslog(LOG_ERR, "err_mach_inject_bundle_couldnt_load_framework_bundle");
            break;
        case err_mach_inject_bundle_couldnt_find_injection_bundle:
            syslog(LOG_ERR, "err_mach_inject_bundle_couldnt_find_injection_bundle");
            break;
        case err_mach_inject_bundle_couldnt_load_injection_bundle:
            syslog(LOG_ERR, "err_mach_inject_bundle_couldnt_load_injection_bundle");
            break;
        case err_mach_inject_bundle_couldnt_find_inject_entry_symbol:
            syslog(LOG_ERR, "err_mach_inject_bundle_couldnt_find_inject_entry_symbol");
            break;
            
        default:
            syslog(LOG_ERR, "Unknown mach inject error: %d", err);
    }
}

- (void)quit
{
    syslog(LOG_NOTICE, "Quitting gracefully");
    self.should_keep_running = NO;
}

- (int)getVersion
{
    return INFINIT_DAEMON_VERSION;
}

@end
