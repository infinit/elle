//
//  IAAppDelegate.m
//  InfinitApplication
//
//  Created by infinit on 10/17/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAAppDelegate.h"

#import <Security/Authorization.h>
#import <ServiceManagement/ServiceManagement.h>
#import <Foundation/NSConnection.h>

@implementation IAAppDelegate


- (void)awakeFromNib {
    NSStatusBar* main_status_bar = [NSStatusBar systemStatusBar];
    self.status_item = [main_status_bar statusItemWithLength:NSVariableStatusItemLength];
    [self.status_item setMenu:self.status_menu];
    [self setDefaultStatus];
    [IAAppDelegate _launchDaemonTest];
    
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
//    self.controller = [DropWindowController getInstance];
//    [self.controller showWindow:self];
}

- (void)setDefaultStatus
{
    NSLog(@"setDefaultStatus");
    if (self.default_status_icon == nil)
    {
        self.default_status_icon = [NSImage imageNamed:@"active_status_icon"];
        [self.default_status_icon setTemplate:YES];
    }
    [self.status_item setImage:self.default_status_icon];
    [self.status_item setHighlightMode:YES];
}

+ (void)_launchDaemonTest
{
    NSLog(@"Launching daemon");
    AuthorizationItem auth = {
        kSMRightBlessPrivilegedHelper,
        0,
        NULL,
        0,
    };
    
    AuthorizationRights rights = {
        1,
        &auth,
    };
    
    AuthorizationFlags flags =
    kAuthorizationFlagDefaults              |
    kAuthorizationFlagInteractionAllowed    |
    kAuthorizationFlagExtendRights          |
    kAuthorizationFlagPreAuthorize
    ;
    
    AuthorizationRef auth_ref = NULL;
    NSString *label = @"io.infinit.InfinitDaemon";

    /* Obtain the right to install privileged helper tools (kSMRightBlessPrivilegedHelper). */
    OSStatus status = AuthorizationCreate(&rights, kAuthorizationEmptyEnvironment, flags, &auth_ref);
    if (status != errAuthorizationSuccess) {
        NSLog(@"Failed to create AuthorizationRef, return code %i", status);
    } else {
        /* This does all the work of verifying the helper tool against the application
         * and vice-versa. Once verification has passed, the embedded launchd.plist
         * is extracted and placed in /Library/LaunchDaemons and then loaded. The
         * executable is placed in /Library/PrivilegedHelperTools.
         */
        CFErrorRef error;
        SMJobRemove(kSMDomainSystemLaunchd, (__bridge CFStringRef)label, auth_ref, YES, &error);
        NSLog(@"About to bless the job");
        Boolean result = SMJobBless(kSMDomainSystemLaunchd, (__bridge CFStringRef)label, auth_ref,  &error);
        if (!result)
        {
            NSLog(@"Couldn't bless the infinit daemon:");
        }
        else
        {
            NSLog(@"COULD bless the infinit");
            id resul = [NSConnection rootProxyForConnectionWithRegisteredName:label host:nil];
            (void)resul;
//            xpc_connection_t connection = xpc_connection_create_mach_service([label UTF8String],
//                                                                             NULL,
//                                                                             XPC_CONNECTION_MACH_SERVICE_PRIVILEGED);
//            if (!connection)
//            {
//                NSLog(@"Cannot connect to %s", [label UTF8String]);
//            }
//            else
//                NSLog(@"Connected to %s", [label UTF8String]);
//            
//            xpc_connection_set_event_handler(connection, ^(xpc_object_t event) {
//                xpc_type_t type = xpc_get_type(event);
//                
//                if (type == XPC_TYPE_ERROR) {
//                    
//                    if (event == XPC_ERROR_CONNECTION_INTERRUPTED) {
//                        NSLog(@"XPC connection interupted.");
//                        
//                    } else if (event == XPC_ERROR_CONNECTION_INVALID) {
//                        NSLog(@"XPC connection invalid, releasing.");
//                        xpc_release(connection);
//                        
//                    } else {
//                        NSLog(@"Unexpected XPC connection error.");
//                    }
//                    
//                } else {
//                    NSLog(@"Unexpected XPC connection event.");
//                }
//            });
//            
//            xpc_connection_resume(connection);
//            
        }
    }
}


@end
