//
//  OOAppDelegate.m
//  Infinit
//
//  Created by Charles Guillot - Infinit on 3/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "AppDelegate.h"
#import "OOInjectorHelper.h"
#import "OOSetupWindowController.h"
#import "OOSetupWindowDelegate.h"
#import "OOPreferencesWindowController.h"
#import <Phone/OOPhone.h>
#import "OOEnvironmentVar.h"

NSString *OOOpenSetupWindowAndStopWatchdog = @"OOOpenSetupWindowAndStopWatchdog";

@implementation AppDelegate

@synthesize window = _window;
@synthesize currentFrame;
@synthesize animTimer;
@synthesize isPending;
@synthesize browserWindowController;
@synthesize isLoginIn;
@synthesize isUpdating;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Init variables
    pendingCount = 0;
    [OOEnvironmentVar setEnvironmentVar];
    self.isUpdating = YES;
    self.isLoginIn = YES;
    // Launch installer process
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(helperInstalled) name:OOHelperInstalled object:nil];
    [OOInjectorHelper launchFinderHelperTools:NO];
}

- (void)helperInstalled {
    [self tryToLogin];
}

- (void)awakeFromNib {
    statusItem = [[[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength] retain];
    [statusItem setMenu:statusMenu];
    [self setDefaultStatusIcon];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(showSetupWindow) name:OOOpenSetupWindowAndStopWatchdog object:nil];
}

- (void)addPending {
    pendingCount += 1;
    if (pendingCount > 0 && !self.isPending){
        currentFrame = 0;
        animTimer = [NSTimer scheduledTimerWithTimeInterval:1.0/7.0 
                                                     target:self 
                                                   selector:@selector(updateStatusItemImageWithTimer:) 
                                                   userInfo:nil 
                                                    repeats:YES];
        self.isPending = YES;
    }
}

- (void)removePending {
    pendingCount -= 1;
    if (pendingCount <= 0 && self.isPending) {
        [animTimer invalidate];
        self.isPending = NO;
        [self setDefaultStatusIcon];
    }
}

- (void)setDefaultStatusIcon {
    if (defaultIcon == nil)
        defaultIcon = [NSImage imageNamed:@"19px-active"];
    [defaultIcon setTemplate:YES];
    [statusItem setImage:defaultIcon];
    [statusItem setHighlightMode:YES];
}

- (void)launch8Watchdog {
    NSLog(@"Launch 8watchdog");
    [[OOPhone getInstance] launchWatchdog];
}

- (void)stop8Watchdog {
    NSLog(@"Stop 8watchdog");
    [[OOPhone getInstance] stopWatchdog];
}

- (void)tryToLogin {
    [statusItem setMenu:statusLoginMenu];
    NSUserDefaults *pref =[NSUserDefaults standardUserDefaults];
    NSString* email = [pref objectForKey:@"Email"];
    NSString* password = [pref objectForKey:@"Password"];
    NSString* machineName = [pref objectForKey:@"ComputerName"];
    BOOL rememberMe = [[pref objectForKey:@"RememberMe"] boolValue];
    
    if (email == nil || password == nil || machineName == nil) {
        [self loginResult:[NSNumber numberWithInt:1]];
        return;
    } else if ([email length] * [password length] * [machineName length] == 0 ) {
        [self loginResult:[NSNumber numberWithInt:1]];
        return;
    }
    
    if (rememberMe) {
        [self addPending];
        [[OOPhone getInstance] loginWithEmail:email 
                                     password:password
                                  machineName:machineName
                              performSelector:@selector(loginResult:) 
                                    forObject:self];
    } else {
        [self loginResult:[NSNumber numberWithInt:1]];
    }
}

- (void)showSetupWindow {
    [self addPending];
    [self stopInfinit];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(userLoggedNotification:) name:OOUserLoggedNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(userLoggedNotification:) name:OOUserUnLoggedNotification object:nil];
    OOSetupWindowController *setupWindowsController = [OOSetupWindowController getInstance];
    [setupWindowsController showWindow:self];
}

- (void)loginResult:(NSNumber *)arg1 {
    int error = [arg1 intValue];
    if (error != 0) {
        NSString *appDomain = [[NSBundle mainBundle] bundleIdentifier];
        [[NSUserDefaults standardUserDefaults] removePersistentDomainForName:appDomain];
        [[NSUserDefaults standardUserDefaults] synchronize];
        NSLog(@"Show setup windows");
        [self showSetupWindow];
    }
    else {
        NSLog(@"Start Infinit");
        self.isLoginIn = NO;
        [self removePending];
        [self readyToStartInfinit];
    }
}

- (void)userLoggedNotification:(NSNotification *)notification {
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:OOUserLoggedNotification
                                                  object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:OOUserUnLoggedNotification
                                                  object:nil];
    NSLog(@"User logged notification");
    if ([notification name] == OOUserLoggedNotification) {
        NSLog(@"User logged");
        [self loginResult:0];
    } else if([notification name] == OOUserUnLoggedNotification) {
        NSLog(@"User unlogged");
        [NSApp terminate:nil];
    }
}

- (void)readyToStartInfinit {
    NSLog(@"Ready to start Infinit");
    if (!self.isLoginIn) {
        NSLog(@"Is loged and is updated");
        [statusItem setMenu:statusMenu];
        [self launch8Watchdog];
        if (!self.browserWindowController)
            self.browserWindowController = [[OOBrowserWindowController alloc] initWithWindowNib];
        [self.browserWindowController loadWindow];
        [[NSNotificationCenter defaultCenter] postNotificationName:OOBrowserWindowControllerSetOnView object:self.browserWindowController];
    }
}

- (void)stopInfinit {
    [self stop8Watchdog];
    if (self.browserWindowController) {
        [[NSNotificationCenter defaultCenter] postNotificationName:OOBrowserWindowControllerSetOffView object:self.browserWindowController];
    }
}

- (void)updateStatusItemImageWithTimer:(NSTimer*)arg1 {
    //get the image for the current frame
    currentFrame = (++currentFrame % 18) + 1;
    NSImage* image = [NSImage imageNamed:[NSString stringWithFormat:@"%ld",currentFrame]];
    [image setTemplate:YES];
    [statusItem setImage:image];
}

- (IBAction)openInfinitNeworks:(id)sender {
    [NSApp activateIgnoringOtherApps:YES];
    [[browserWindowController window] orderFrontRegardless];
}

- (IBAction)openPreferenceWindow:(id)sender {
    OOPreferencesWindowController* preferencesWindowController = [OOPreferencesWindowController getInstance];
    [preferencesWindowController showWindow:self];
}

- (IBAction)launchWebsite:(id)sender {
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://www.infinit.io"]];
}

- (IBAction)launchHelpCenter:(id)sender {
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://www.infinit.io/help"]];
}

- (void)applicationWillTerminate:(NSNotification *)notification {
    [self stop8Watchdog];
}

@end
