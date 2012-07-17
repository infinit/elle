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
#import <Phone/OOPhone.h>
#import <FinderPanel/OOBrowserWindowController.h>

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
    // Launch installer process
    //[OOInjectorHelper launchFinderHelperTools:YES];
    
    //[self update];
    [self tryToLogin];
    //OOBrowserWindowController* aaa = [[OOBrowserWindowController alloc] initWithWindowNib];
    //[aaa showWindow:self];
}

- (void)awakeFromNib
{
    statusItem = [[[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength] retain];
    [statusItem setMenu:statusMenu];
    NSImage *icon = [NSImage imageNamed:[NSString stringWithFormat:@"19px-active",currentFrame]];
    [icon setTemplate:YES];
    [statusItem setImage:icon];
    [statusItem setHighlightMode:YES];
}

- (void)addPending {
    pendingCount += 1;
    if (pendingCount > 0 && !self.isPending){
        currentFrame = 0;
        animTimer = [NSTimer scheduledTimerWithTimeInterval:1.0/7.0 target:self selector:@selector(updateStatusItemImageWithTimer:) userInfo:nil repeats:YES];
    }
}

- (void)removePending {
    pendingCount -= 1;
    if (pendingCount <= 0 && self.isPending)
        [animTimer invalidate];
}

- (void)launch8infinit
{
    NSString *installerPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingString:@"/bin/8updater"];
    [NSTask launchedTaskWithLaunchPath:installerPath arguments:[NSArray arrayWithObjects:nil]];
}

- (void)tryToLogin
{
    [self addPending];
    [statusItem setMenu:statusLoginMenu];
    NSUserDefaults *pref;
    pref=[NSUserDefaults standardUserDefaults];
    NSString* email = [pref objectForKey:@"Email"];
    NSString* password = [pref objectForKey:@"Password"];
    NSString* computerName = [pref objectForKey:@"ComputerName"];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(userLoggedNotification:) name:OOUserLoggedNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(userLoggedNotification:) name:OOUserUnLoggedNotification object:nil];
    
    [[OOPhone getInstance] loginWithEmail:email 
                                 password:password
                          performSelector:@selector(loginResult:) 
                                forObject:self];
    
}

- (void)loginResult:(NSNumber *)arg1 {
    if (arg1 > 0) {
        OOSetupWindowController *setupWindowsController = [[OOSetupWindowController alloc] init];
        [setupWindowsController showWindow:self];
    }
    else {
        [statusItem setMenu:statusMenu];
        [self removePending];
    }
}

- (void)userLoggedNotification:(NSNotification *)notification {
    if ([notification name] == OOUserLoggedNotification) {
        [statusItem setMenu:statusMenu];
        [self removePending];
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
        }
        else {
            [statusItem setTitle:[NSString stringWithFormat:@"(%00.f%%)", [progress floatValue]*100]];
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

- (IBAction)installInjectBundle:(id)sender
{
    [OOInjectorHelper launchFinderHelperTools:YES];
}

- (IBAction)injectBundle:(id)sender
{
    [OOInjectorHelper launchFinderHelperTools:NO];
}

- (IBAction)launchWebsite:(id)sender
{
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://www.infinit.io"]];
}

- (IBAction)launchHelpCenter:(id)sender
{
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://www.infinit.io/help"]];
}

@end
