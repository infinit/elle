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
#define PANEL_WIDTH 400

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
    NSRect screenRect = [[[NSScreen screens] objectAtIndex:0] frame];
    NSRect statusRect = [self statusRectForWindow:self];
    
    NSRect panelRect = [self frame];
    panelRect.size.width = PANEL_WIDTH;
    panelRect.origin.x = roundf(NSMidX(statusRect) - NSWidth(panelRect) / 2);
    panelRect.origin.y = NSMaxY(statusRect) - NSHeight(panelRect);
    
    if (NSMaxX(panelRect) > (NSMaxX(screenRect) - ARROW_HEIGHT))
        panelRect.origin.x -= NSMaxX(panelRect) - (NSMaxX(screenRect) - ARROW_HEIGHT);
    
    NSLog(@"Showing panel at (x=%f, y=%f, w=%f, h=%f)",
          panelRect.origin.x,
          panelRect.origin.y,
          panelRect.size.width,
          panelRect.size.height);
    
    [NSApp activateIgnoringOtherApps:YES];
    [self setAlphaValue:0];
    [self setFrame:statusRect display:YES];
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
        statusRect = status_item_view.absoluteFrame;
        statusRect.origin.y = NSMinY(statusRect) - NSHeight(statusRect);
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
