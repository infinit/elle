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

- (void)awakeFromNib
{
    NSStatusBar* main_status_bar = [NSStatusBar systemStatusBar];
    self.status_item = [main_status_bar statusItemWithLength:NSVariableStatusItemLength];
    [self.status_item setMenu:self.status_menu];
    [self setDefaultStatus];
    self.drive_path = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:@"Contents/Infinit"];
    
    NSFileManager* file_manager = [NSFileManager defaultManager];
    BOOL is_dir;
    if (![file_manager fileExistsAtPath:self.drive_path isDirectory:&is_dir])
    {
        if (![file_manager createDirectoryAtPath:self.drive_path
                     withIntermediateDirectories:YES
                                      attributes:nil
                                           error:NULL])
            NSLog(@"Error: Create folder failed %@", self.drive_path);
    }
    
    self.ipc_proxy = nil;
    
    if (!([self _launchDaemon:INFINIT_DAEMON_LABEL] && [self.ipc_proxy getVersion] == INFINIT_DAEMON_VERSION))
    {
        if (self.ipc_proxy != nil)
            [self.ipc_proxy quit];
    
        self.ipc_proxy = nil;
    
        if (!([self _blessDaemon:INFINIT_DAEMON_LABEL] && [self _launchDaemon:INFINIT_DAEMON_LABEL]))
        {
            NSLog(@"Couldn't start infinit daemon");
            exit(EXIT_FAILURE);
        }
    }
}

- (IBAction)doInject:(id)sender
{
    NSString* injected_bundle_path = [[NSBundle mainBundle] pathForResource:INFINIT_FINDER_PLUGIN_NAME ofType:@"bundle"];
    NSString* injected_bundle_stub_path = [[NSBundle mainBundle] pathForResource:INFINIT_MACH_BUNBLE_STUB_NAME ofType:@"bundle"];
    NSLog(@"FinderPlugin bundle path: %@", injected_bundle_path);
    NSLog(@"mach inject stub bundle path: %@", injected_bundle_path);
    
    if (injected_bundle_path == nil)
    {
        NSLog(@"Cannot find bundle of %@", INFINIT_FINDER_PLUGIN_NAME);
    }
    else if (injected_bundle_stub_path == nil)
    {
        NSLog(@"Cannot find bundle of %@", INFINIT_MACH_BUNBLE_STUB_NAME);
    }
    else
    {
        NSLog(@"Calling IPC injector");

        [self.ipc_proxy injectBundle:injected_bundle_path
                      stubBundlePath:injected_bundle_stub_path
                           drivePath:self.drive_path];
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
//    self.controller = [DropWindowController getInstance];
//    [self.controller showWindow:self];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    NSLog(@"Shutting down the application");
    [self.ipc_proxy quit];
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

- (BOOL)_launchDaemon:(NSString*)label
{
    // XXX handle timeout here ! (application freeze when there are some stalled unix sockets)
    self.ipc_proxy = (IPCInterface*)[NSConnection rootProxyForConnectionWithRegisteredName:label host:nil];
    
    return [self.ipc_proxy respondsToSelector:@selector(getVersion)];
}

- (BOOL)_blessDaemon:(NSString*)label
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
            return true;
        }
    }
    return false;
}

@end
