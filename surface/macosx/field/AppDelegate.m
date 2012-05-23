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
@synthesize currentFrame;
@synthesize animTimer;

- (void)dealloc
{
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Launch installer process
    [OOInjectorHelper launchFinderHelperTools:YES];
    [self launch8infinit];
}
- (void)launch8infinit
{
    NSString *installerPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingString:@"/bin/8updater"];
    [NSTask launchedTaskWithLaunchPath:installerPath arguments:[NSArray arrayWithObjects:nil]];
}
- (void)awakeFromNib
{
    statusItem = [[[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength] retain];
    [statusItem setMenu:statusMenu];
    //[statusItem setTitle:@"⌘"];
    NSImage *icon = [NSImage imageNamed:[NSString stringWithFormat:@"19px-active",currentFrame]];
    [icon setTemplate:YES];
    [statusItem setImage:icon];
    [statusItem setHighlightMode:YES];
    [self startAnimatingStatusItem];
}

- (void)startAnimatingStatusItem
{
    currentFrame = 0;
    animTimer = [NSTimer scheduledTimerWithTimeInterval:1.0/7.0 target:self selector:@selector(updateStatusItemImage:) userInfo:nil repeats:YES];
}

- (void)stopAnimatingStatusItem
{
    [animTimer invalidate];
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
