//
//  IANotificationPanel.m
//  FinderWindowApplication
//
//  Created by infinit on 11/9/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IANotificationPanel.h"
#import "IAStatusBarController.h"
#import "IAStatusItemView.h"

#define PANEL_HEIGHT 400
#define PANEL_WIDTH 800

#define ARROW_HEIGHT 20

@implementation IANotificationPanel
{
@private
    IAStatusBarController* _status_bar_controller;
}

- (void)awakeFromNib
{
    [self setAcceptsMouseMovedEvents:YES];
    [self setLevel:NSPopUpMenuWindowLevel];
    [self setOpaque:NO];
    [self setBackgroundColor:[NSColor clearColor]];
    
    // Resize panel
    NSRect panelRect = [self frame];
    panelRect.size.height = PANEL_HEIGHT;
    [self setFrame:panelRect display:NO];
}

- (void)setStatusBarController:(id)controller
{
    assert(controller != nil);
    _status_bar_controller = controller;
}

- (void)show
{
    IAStatusItemView *status_item_view = [_status_bar_controller getStatusItemView];
    NSRect frame = [status_item_view frame];
    frame.origin = [status_item_view.window convertBaseToScreen:frame.origin];
    frame.origin.x -= PANEL_WIDTH / 2;
    frame.origin.y -= PANEL_HEIGHT;//frame.size.height;
    frame.size.width = PANEL_WIDTH;
    frame.size.height = PANEL_HEIGHT;

    NSLog(@"Showing panel at (x=%f, y=%f, w=%f, h=%f)",
          frame.origin.x,
          frame.origin.y,
          frame.size.width,
          frame.size.height);
    
    [NSApp activateIgnoringOtherApps:NO];
    [self setHidesOnDeactivate:YES];
   // [self setAlphaValue:0];
    [self setFrame:frame display:YES];
    [self makeKeyAndOrderFront:self];
}

- (void)hide
{
    [self orderOut:nil];
}

- (NSRect)statusRectForWindow:(NSWindow *)window
{
    NSRect screenRect = [[[NSScreen screens] objectAtIndex:0] frame];
    NSRect statusRect = NSZeroRect;
    
    IAStatusItemView *status_item_view = [_status_bar_controller getStatusItemView];
    
    if (status_item_view)
    {
        NSRect frame = [status_item_view frame];
        frame.origin = [status_item_view.window convertBaseToScreen:frame.origin];
        frame.origin.y = frame.size.height;
        statusRect = frame;

        NSLog(@"statusRect computed from status item view (x=%f, y=%f, w=%f, h=%f",
              frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
    }
    else
    {
        statusRect.size = NSMakeSize([_status_bar_controller getStatusItemView].width,
                                     [[NSStatusBar systemStatusBar] thickness]);
        statusRect.origin.x = roundf((NSWidth(screenRect) - NSWidth(statusRect)) / 2);
        statusRect.origin.y = NSHeight(screenRect) - NSHeight(statusRect) * 2;
    }
    return statusRect;
}

@end
