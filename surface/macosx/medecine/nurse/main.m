//
//  main.m
//  InfinitHelperTool
//
//  Created by Charles Guillot - Infinit on 3/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OONurseInjector.h"
#import <syslog.h>
#import <launch.h>

int main (int argc, const char * argv[])
{
    
    @autoreleasepool {
        
        syslog(LOG_NOTICE, "ElevatorDOHelper launched (uid: %d, euid: %d, pid: %d)", getuid(), geteuid(), getpid());
        
        launch_data_t req = launch_data_new_string(LAUNCH_KEY_CHECKIN);
        launch_data_t resp = launch_msg(req);
        launch_data_t machData = launch_data_dict_lookup(resp, LAUNCH_JOBKEY_MACHSERVICES);
        launch_data_t machPortData = launch_data_dict_lookup(machData, "io.infinit.Nurse.helper");
        
        mach_port_t mp = launch_data_get_machport(machPortData);
        launch_data_free(req);
        launch_data_free(resp);
        
        NSMachPort *rp = [[NSMachPort alloc] initWithMachPort:mp];
        NSConnection *c = [NSConnection connectionWithReceivePort:rp sendPort:nil];
        
        OONurseInjector *obj = [[OONurseInjector alloc] init];
        [c setRootObject:obj];
        [[NSRunLoop currentRunLoop] run];
        
    }
    return 0;
}

