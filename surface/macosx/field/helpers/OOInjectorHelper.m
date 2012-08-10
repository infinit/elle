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
#import "OONurseManager.h"

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
    // Connect
    NSConnection *c = [NSConnection connectionWithRegisteredName:@"io.infinit.Nurse.helper" host:nil]; 
    OONurseManager *proxy = (OONurseManager *)[c rootProxy];
    
    NSString *currentDir = [[NSBundle mainBundle] bundlePath];
    
    NSLog(@"launch helper for : %@", currentDir);
    BOOL returnValue = [proxy manage:currentDir];
    
    if (returnValue) {
        NSLog(@"Helper launched");
    }
    else {
        NSLog(@"Helper failled to launch");
    }
    
    return returnValue;
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
        SMJobRemove(kSMDomainSystemLaunchd, (CFStringRef)label, authRef, YES, (CFErrorRef *)error);
		result = SMJobBless(kSMDomainSystemLaunchd, (CFStringRef)label, authRef, (CFErrorRef *)error);
	}
	
	return result;
}

@end
