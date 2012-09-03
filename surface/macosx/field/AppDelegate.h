//
//  OOAppDelegate.h
//  Infinit
//
//  Created by Charles Guillot - Infinit on 3/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <FinderPanel/OOBrowserWindowController.h>

extern NSString *OOOpenSetupWindowAndStopWatchdog;

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
    NSWindow                    *window;
    IBOutlet NSMenu             *statusMenu;
    IBOutlet NSMenu             *statusLoginMenu;
    NSStatusItem                *statusItem;
    NSInteger                   currentFrame;
    NSTimer                     *animTimer;
    NSImage                     *defaultIcon;
    int                         pendingCount;
    BOOL                        isPending;
    BOOL                        isUpdating;
    BOOL                        isLoginIn;
    OOBrowserWindowController   *browserWindowController;
}

@property(assign)   IBOutlet NSWindow               *window;
@property(assign)   NSInteger                       currentFrame;
@property           NSTimer                         *animTimer;
@property           BOOL                            isPending;
@property           BOOL                            isUpdating;
@property           BOOL                            isLoginIn;
@property(retain)   OOBrowserWindowController       *browserWindowController;

- (void)launch8Watchdog;

- (void)addPending;
- (void)removePending;
- (void)updateStatusItemImageWithTimer:(NSTimer*)arg1;

- (IBAction)openInfinitNeworks:(id)sender;
- (IBAction)launchWebsite:(id)sender;
- (IBAction)launchHelpCenter:(id)sender;
- (void)showSetupWindow;
@end
