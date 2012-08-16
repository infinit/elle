//
//  main.m
//  InfinitHelperTool
//
//  Created by Charles Guillot - Infinit on 3/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OONurseManager.h"
#import <syslog.h>
#import <launch.h>

void SIGTERM_handler(const int sigid)
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
        
       syslog(LOG_NOTICE, "Nurse helper launched (uid: %d, euid: %d, pid: %d)", getuid(), geteuid(), getpid());
       
       signal(SIGTERM, (sig_t)SIGTERM_handler);
       
       launch_data_t req = launch_data_new_string(LAUNCH_KEY_CHECKIN);
       launch_data_t resp = launch_msg(req);
       launch_data_t machData = launch_data_dict_lookup(resp, LAUNCH_JOBKEY_MACHSERVICES);
       launch_data_t machPortData = launch_data_dict_lookup(machData, "io.infinit.Nurse.helper");
        
       mach_port_t mp = launch_data_get_machport(machPortData);
       launch_data_free(req);
       launch_data_free(resp);
        
       NSMachPort *rp = [[NSMachPort alloc] initWithMachPort:mp];
       NSConnection *c = [NSConnection connectionWithReceivePort:rp sendPort:nil];
       
       OONurseManager *obj = [[OONurseManager alloc] init];
       [c setRootObject:obj];
       
       [[NSRunLoop currentRunLoop] run];
        
    }
    return 0;
}

