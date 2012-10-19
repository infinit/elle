//
//  main.m
//  InfinitHelperTool
//
//  Created by Charles Guillot - Infinit on 3/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "IPCInterface.h"
#import <syslog.h>
#import <launch.h>

void sigterm(const int sigid)
{
	CFRunLoopRef rl = [[NSRunLoop currentRunLoop] getCFRunLoop];
	if (rl == NULL) {
		exit(1); // Oops. Exit!
	} else {
		CFRunLoopStop(rl);
		exit(0);
	}
}

int main (int argc, const char * argv[])
{
   @autoreleasepool {
        
       syslog(LOG_NOTICE, "Starting daemon (uid: %d, euid: %d, pid: %d)", getuid(), geteuid(), getpid());
       
       signal(SIGTERM, (sig_t)sigterm);
       launch_data_t req = launch_data_new_string(LAUNCH_KEY_CHECKIN);
       launch_data_t resp = launch_msg(req);
       launch_data_t machData = launch_data_dict_lookup(resp, LAUNCH_JOBKEY_MACHSERVICES);
       launch_data_t machPortData = launch_data_dict_lookup(machData, "io.infinit.InfinitDaemon");
       mach_port_t mp = launch_data_get_machport(machPortData);
       
       launch_data_free(req);
       launch_data_free(resp);
       
       NSMachPort* rp = [[NSMachPort alloc] initWithMachPort:mp];
       NSConnection* conn = [NSConnection connectionWithReceivePort:rp sendPort:nil];

       
       IPCInterface* interface = [[IPCInterface alloc] init];
     /*  [interface injectBundle:@"/Users/infinit/Library/Developer/Xcode/DerivedData/InfinitApplication-bfgrqdidykomoebfxsixhhcxarqj/Build/Products/Debug/InfinitApplication.app/Contents/Resources/FinderPlugin.bundle"
                stubBundlePath:@"/Users/infinit/Library/Developer/Xcode/DerivedData/InfinitApplication-bfgrqdidykomoebfxsixhhcxarqj/Build/Products/Debug/InfinitApplication.app/Contents/Resources/mach_inject_bundle_stub.bundle"];*/
       [conn setRootObject:interface];
       
        NSRunLoop* loop = [NSRunLoop currentRunLoop];
       
        while (interface.should_keep_running && [loop runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]])
            ;
    }
    return 0;
}

