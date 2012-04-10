//
//  OOHelpers.m
//  Infinit
//
//  Created by Charles Guillot - Infinit on 3/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOInjectorHelper.h"
#import <ServiceManagement/ServiceManagement.h>
#import <Security/Authorization.h>
#import "OONurseInjector.h"

@implementation OOInjectorHelper

+ (BOOL)launchFinderHelperTools:(BOOL)blessHelper
{
    if (blessHelper)
    {
        NSError *error = nil;
        if (![OOInjectorHelper blessHelperWithLabel:@"io.infinit.Nurse.helper" error:&error]) {
            NSLog(@"Failed to bless helper. Error: %@", error);
            return NO;
        }
    }
    // Connect to D
    NSConnection *c = [NSConnection connectionWithRegisteredName:@"io.infinit.Nurse.helper" host:nil]; 
    OOInjectorHelper *proxy = (OOInjectorHelper *)[c rootProxy];
    
    NSString *currentDir = [[NSFileManager defaultManager] currentDirectoryPath];

    
    BOOL returnValie = [proxy injectWithAppPath:currentDir forceInstall:YES];
    return returnValie;
    // Get name from DO
//    @try {
//    xpc_connection_t connection = xpc_connection_create_mach_service("io.infinit.HelperTools.helper", NULL, XPC_CONNECTION_MACH_SERVICE_PRIVILEGED);
//    
//    if (!connection) {
//        NSLog(@"Failed to create XPC connection.");
//        return NO;
//    }
//    
//    xpc_connection_set_event_handler(connection, ^(xpc_object_t event) {
//        xpc_type_t type = xpc_get_type(event);
//        
//        if (type == XPC_TYPE_ERROR) {
//            
//            if (event == XPC_ERROR_CONNECTION_INTERRUPTED) {
//                NSLog(@"XPC connection interupted.");
//                
//            } else if (event == XPC_ERROR_CONNECTION_INVALID) {
//                NSLog(@"XPC connection invalid, releasing.");
//                xpc_release(connection);
//                
//            } else {
//                NSLog(@"Unexpected XPC connection error.");
//            }
//            
//        } else {
//            NSLog(@"Unexpected XPC connection event.");
//        }
//    });
//    
//    xpc_connection_resume(connection);
//    
//    xpc_object_t message = xpc_dictionary_create(NULL, NULL, 0);
//    const char* request = "##INSTALLINJECT##";
//    xpc_dictionary_set_string(message, "request", request);
//    
//    NSLog(@"Sending request: %s", request);
//    
//    xpc_connection_send_message_with_reply(connection, message, dispatch_get_main_queue(), ^(xpc_object_t event) {
//        @try {
//            NSLog(@"Receiving response...");
//            const char* response = xpc_dictionary_get_string(event, "reply");
//            if ( [[NSString stringWithUTF8String: response] isEqualToString: @"##OK##" ] )
//            {
//                xpc_object_t message = xpc_dictionary_create(NULL, NULL, 0);
//                const char* request = "##EXIT##";
//                xpc_dictionary_set_string(message, "request", request);
//                xpc_connection_send_message(connection, message);
//            }
//            NSLog(@"Received response: %s.", response);
//        }
//        @catch (NSException *exception) {
//            NSLog(@"Exception occured when receiving response: %@", exception);
//        }
//    });
//    return YES;
//    }
//    @catch (NSException *exception) {
//        NSLog(@"Connection exception: %@.", exception);
//        return NO;
//    }
}

+ (BOOL)blessHelperWithLabel:(NSString *)label error:(NSError **)error
{
	BOOL result = NO;
    
	AuthorizationItem authItem		= { kSMRightBlessPrivilegedHelper, 0, NULL, 0 };
	AuthorizationRights authRights	= { 1, &authItem };
	AuthorizationFlags flags		=	kAuthorizationFlagDefaults				| 
                                        kAuthorizationFlagInteractionAllowed	|
                                        kAuthorizationFlagPreAuthorize			|
                                        kAuthorizationFlagExtendRights;
    
	AuthorizationRef authRef = NULL;
	
	/* Obtain the right to install privileged helper tools (kSMRightBlessPrivilegedHelper). */
	OSStatus status = AuthorizationCreate(&authRights, kAuthorizationEmptyEnvironment, flags, &authRef);
	if (status != errAuthorizationSuccess) {
        NSLog(@"Failed to create AuthorizationRef. Error code: %d", status);
        
	} else {
		/* This does all the work of verifying the helper tool against the application
		 * and vice-versa. Once verification has passed, the embedded launchd.plist
		 * is extracted and placed in /Library/LaunchDaemons and then loaded. The
		 * executable is placed in /Library/PrivilegedHelperTools.
		 */
		result = SMJobBless(kSMDomainSystemLaunchd, (CFStringRef)label, authRef, (CFErrorRef *)error);
	}
	
	return result;
}

@end
