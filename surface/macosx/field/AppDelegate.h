//
//  OOAppDelegate.h
//  Infinit
//
//  Created by Charles Guillot - Infinit on 3/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
    NSWindow *window;
    IBOutlet NSMenu *statusMenu;
    NSStatusItem *statusItem;
    NSInteger currentFrame;
    NSTimer *animTimer;
}

@property (assign) IBOutlet NSWindow *window;
@property (assign) NSInteger currentFrame;
@property (assign) NSTimer *animTimer;

- (void)launch8infinit;

- (void)startAnimatingStatusItem;
- (void)stopAnimatingStatusItem;
- (void)updateStatusItemImageWithTimer:(NSTimer*)arg1;

- (IBAction)openInfinitNeworks:(id)sender;
- (IBAction)installInjectBundle:(id)sender;
- (IBAction)injectBundle:(id)sender;
- (IBAction)launchWebsite:(id)sender;
- (IBAction)launchHelpCenter:(id)sender;



@end
