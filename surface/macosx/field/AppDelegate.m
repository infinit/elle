//
//  OOAppDelegate.m
//  Infinit
//
//  Created by Charles Guillot - Infinit on 3/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "AppDelegate.h"
#import "OOInjectorHelper.h"

@implementation AppDelegate

@synthesize window = _window;

- (void)dealloc
{
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Launch installer process
    [self launch8infinit];
}
-(void)launch8infinit
{
    NSString *installerPath = [[NSBundle mainBundle] pathForResource:@"8installer" ofType:nil];
    [NSTask launchedTaskWithLaunchPath:installerPath arguments:Nil];
}
-(void)awakeFromNib
{
    statusItem = [[[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength] retain];
    [statusItem setMenu:statusMenu];
    [statusItem setTitle:@"⌘"];
    //[statusItem setImage:[[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"Infinit" ofType:@"icns"]]];
    [statusItem setHighlightMode:YES];
    //[statusItem setLength:20];
}

-(IBAction)installInjectBundle:(id)sender
{
    [OOInjectorHelper launchFinderHelperTools:YES];
}

-(IBAction)injectBundle:(id)sender
{
    [OOInjectorHelper launchFinderHelperTools:NO];
}

-(IBAction)launchWebsite:(id)sender
{
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://www.infinit.io"]];
}

-(IBAction)launchHelpCenter:(id)sender
{
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://www.infinit.io/help"]];
}

-(IBAction)openPreferences:(id)sender
{
    [[NSWorkspace sharedWorkspace] openFile:@ "/Library/PreferencePanes/Flash Player.prefPane"];
}

@end
