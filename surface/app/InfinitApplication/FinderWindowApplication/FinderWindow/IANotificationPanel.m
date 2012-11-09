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

@implementation IANotificationPanel
{
@private
    IAStatusBarController* _status_bar_controller;
}

- (void)setStatusBarController:(id)controller
{
    assert(controller != nil);
    _status_bar_controller = controller;
}

- (void)show
{
    [self makeKeyAndOrderFront:nil];
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
