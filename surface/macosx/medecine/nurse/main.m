//
//  main.m
//  InfinitHelperTool
//
//  Created by Charles Guillot - Infinit on 3/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "IHTInjector.h"
#import <syslog.h>
#import <launch.h>

//static void __XPC_Peer_Event_Handler(xpc_connection_t connection, xpc_object_t event) {
//   syslog(LOG_NOTICE, "Received event in helper.");
//    
//	xpc_type_t type = xpc_get_type(event);
//    
//	if (type == XPC_TYPE_ERROR) {
//		if (event == XPC_ERROR_CONNECTION_INVALID) {
//            syslog(LOG_NOTICE, "Injector : connection invalid");
//            exit(0);
//			// The client process on the other end of the connection has either
//			// crashed or cancelled the connection. After receiving this error,
//			// the connection is in an invalid state, and you do not need to
//			// call xpc_connection_cancel(). Just tear down any associated state
//			// here.
//            
//		} else if (event == XPC_ERROR_TERMINATION_IMMINENT) {
//            syslog(LOG_NOTICE, "Injector : connection terminate");
//            exit(0);
//			// Handle per-connection termination cleanup.
//		}
//        
//	} else {
//        const char* request = xpc_dictionary_get_string(event, "request");
//        if ( 
//            [[NSString stringWithUTF8String: request] isEqualToString: @"##INSTALLINJECT##" ] ||
//            [[NSString stringWithUTF8String: request] isEqualToString: @"##INJECT##" ]
//            )
//        {
//            syslog(LOG_NOTICE, "Instanciate Injector");
//            BOOL injected = 
//            [IHTInjector inject: [[NSString stringWithUTF8String: request] isEqualToString: @"##INSTALLINJECT##" ]];
//        
//            xpc_connection_t remote = xpc_dictionary_get_remote_connection(event);
//        
//            xpc_object_t reply = xpc_dictionary_create_reply(event);
//            xpc_dictionary_set_string(reply, "reply", injected ? "##OK##" : "##FAIL##");
//            xpc_connection_send_message(remote, reply);
//            xpc_release(reply);
//        }
//        else if ( [[NSString stringWithUTF8String: request] isEqualToString: @"##EXIT##" ] )
//        {
//            syslog(LOG_NOTICE, "Exit");
//            exit(0);
//        }
//	}
//}
//
//static void __XPC_Connection_Handler(xpc_connection_t connection)  {
//    syslog(LOG_NOTICE, "Configuring message event handler for helper.");
//    
//	xpc_connection_set_event_handler(connection, ^(xpc_object_t event) {
//		__XPC_Peer_Event_Handler(connection, event);
//	});
//    
//	xpc_connection_resume(connection);
//}
//
//int main (int argc, const char * argv[])
//{
//    syslog(LOG_NOTICE, "Helper launching...");
//    xpc_connection_t service = xpc_connection_create_mach_service("io.infinit.HelperTools.helper",
//                                                                  dispatch_get_main_queue(),
//                                                                  XPC_CONNECTION_MACH_SERVICE_LISTENER);
//    
//    if (!service) {
//        syslog(LOG_NOTICE, "Failed to create service.");
//        exit(EXIT_FAILURE);
//    }
//    syslog(LOG_NOTICE, "Configuring connection event handler for helper");
//    xpc_connection_set_event_handler(service, ^(xpc_object_t connection) {
//        __XPC_Connection_Handler(connection);
//    });
//    
//    xpc_connection_resume(service);
//    
//    dispatch_main();
//    
//    xpc_release(service);
//    
//    return EXIT_SUCCESS;
//}
int main (int argc, const char * argv[])
{
    
    @autoreleasepool {
        
        syslog(LOG_NOTICE, "ElevatorDOHelper launched (uid: %d, euid: %d, pid: %d)", getuid(), geteuid(), getpid());
        
        launch_data_t req = launch_data_new_string(LAUNCH_KEY_CHECKIN);
        launch_data_t resp = launch_msg(req);
        launch_data_t machData = launch_data_dict_lookup(resp, LAUNCH_JOBKEY_MACHSERVICES);
        launch_data_t machPortData = launch_data_dict_lookup(machData, "io.infinit.HelperTools.helper");
        
        mach_port_t mp = launch_data_get_machport(machPortData);
        launch_data_free(req);
        launch_data_free(resp);
        
        NSMachPort *rp = [[NSMachPort alloc] initWithMachPort:mp];
        NSConnection *c = [NSConnection connectionWithReceivePort:rp sendPort:nil];
        
        IHTInjector *obj = [IHTInjector new];
        [c setRootObject:obj];
        [[NSRunLoop currentRunLoop] run];
        
    }
    return 0;
}

