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
#import <FinderPanel/OOBrowserWindowController.h>
#import "OOEnvironmentVar.h"

NSString *OOOpenSetupWindowAndStopWatchdog = @"OOOpenSetupWindowAndStopWatchdog";

@implementation AppDelegate

@synthesize window = _window;
@synthesize currentFrame;
@synthesize animTimer;
@synthesize isPending;

- (void)dealloc
{
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    pendingCount = 0;
    [OOEnvironmentVar setEnvironmentVar];
    // Launch installer process
    [OOInjectorHelper launchFinderHelperTools:YES];
    
    [self update];
    [self tryToLogin];
}

- (void)awakeFromNib
{
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
    if (defaultIcon == nil) {
        defaultIcon = [NSImage imageNamed:[NSString stringWithFormat:@"19px-active",currentFrame]];
    }
    [defaultIcon setTemplate:YES];
    [statusItem setImage:defaultIcon];
    [statusItem setHighlightMode:YES];
}

- (void)launch8Watchdog
{
    [[OOPhone getInstance] launchWatchdog];
}

- (void)tryToLogin
{
    [statusItem setMenu:statusLoginMenu];
    NSUserDefaults *pref;
    pref=[NSUserDefaults standardUserDefaults];
    NSString* email = [pref objectForKey:@"Email"];
    NSString* password = [pref objectForKey:@"Password"];
    BOOL rememberMe = [[pref objectForKey:@"RememberMe"] boolValue];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(userLoggedNotification:) name:OOUserLoggedNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(userLoggedNotification:) name:OOUserUnLoggedNotification object:nil];
    
    if (rememberMe) {
        [self addPending];
        [[OOPhone getInstance] loginWithEmail:email 
                                     password:password
                              performSelector:@selector(loginResult:) 
                                    forObject:self];
    } else {
        [self loginResult:[NSNumber numberWithInt:1]];
    }
    
}

- (void)showSetupWindow {
    [self addPending];
    OOSetupWindowController *setupWindowsController = [OOSetupWindowController getInstance];
    [setupWindowsController showWindow:self];
}

- (void)loginResult:(NSNumber *)arg1 {
    int error = [arg1 intValue];
    if (error != 0) {
        [self showSetupWindow];
    }
    else {
        [statusItem setMenu:statusMenu];
        [self removePending];
        [self launch8Watchdog];
        OOBrowserWindowController* aaa = [[OOBrowserWindowController alloc] initWithWindowNib];
        [aaa showWindow:self];
    }
}

- (void)userLoggedNotification:(NSNotification *)notification {
    if ([notification name] == OOUserLoggedNotification) {
        [self loginResult:0];
    }
    else if([notification name] == OOUserUnLoggedNotification) {
        [NSApp terminate:nil];
    }
}

- (void)update {
    [self addPending];
    [[NSNotificationCenter defaultCenter] addObserver:self 
                                             selector:@selector(updateProgessChangedNotification:) 
                                                 name:OOUpdateProgessChangedNotification 
                                               object:nil];
    [[OOPhone getInstance] update];
}

- (void)updateProgessChangedNotification:(NSNotification *)notification {
    if ([notification name] == OOUpdateProgessChangedNotification) {
        NSNumber* progress = [[notification userInfo] objectForKey:@"progress"];
        if ([progress floatValue] == 1) {
            [statusItem setTitle:nil];
            [[NSNotificationCenter defaultCenter] removeObserver:self
                                                            name:OOUpdateProgessChangedNotification 
                                                          object:nil];
            [self removePending];
        } else if ([progress floatValue] >= 0){
            [statusItem setTitle:[NSString stringWithFormat:@"(%00.f%%)", [progress floatValue]*100]];
        } else {
            [statusItem setTitle:@"Error when downloading"];
            [[NSNotificationCenter defaultCenter] removeObserver:self
                                                            name:OOUpdateProgessChangedNotification
                                                          object:nil];
            [self removePending];
        }
        
    }
}

- (void)updateStatusItemImageWithTimer:(NSTimer*)arg1
{
    //get the image for the current frame
    currentFrame = (++currentFrame % 18) + 1;
    NSImage* image = [NSImage imageNamed:[NSString stringWithFormat:@"%d",currentFrame]];
    [image setTemplate:YES];
    [statusItem setImage:image];
}

- (IBAction)openInfinitNeworks:(id)sender
{
    [[NSWorkspace sharedWorkspace] openFile:@"/Users/charlesguillot/.config/infinit/Infinit"];
}

- (IBAction)openPreferenceWindow:(id)sender
{
    OOPreferencesWindowController* preferencesWindowController = [OOPreferencesWindowController getInstance];
    [preferencesWindowController showWindow:self];
}
/*
- (IBAction)installInjectBundle:(id)sender
{
    [OOInjectorHelper launchFinderHelperTools:YES];
}

- (IBAction)injectBundle:(id)sender
{
    [OOInjectorHelper launchFinderHelperTools:NO];
}*/

- (IBAction)launchWebsite:(id)sender
{
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://www.infinit.io"]];
}

- (IBAction)launchHelpCenter:(id)sender
{
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://www.infinit.io/help"]];
}

@end
