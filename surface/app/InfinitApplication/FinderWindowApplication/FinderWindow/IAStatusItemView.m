//
//  IAStatusItemView.m
//  FinderWindowApplication
//
//  Created by infinit on 11/8/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAStatusItemView.h"

@implementation IAStatusItemView
{
@private
    NSStatusItem*   _status_item;
    BOOL            _highlighted;
    NSImage*        _image;
    NSImage*        _alternate_image;
}

@synthesize width = _width;
@synthesize height = _height;

- (id)initWithStatusItem:(NSStatusItem*)status_item
               withImage:(NSImage*)image
       andAlternateImage:(NSImage*)alternate_image
{
    _image = image;
    _alternate_image = alternate_image;
    _width = [image size].width;//[status_item length];
    _height = [[NSStatusBar systemStatusBar] thickness];
    NSLog(@"Init with size = (%f, %f)", _width, _height);
    NSRect rect = NSMakeRect(0.0, 0.0, _width, _height);
    self = [super initWithFrame:rect];
    
    if (self != nil)
    {
        _status_item = status_item;
        _status_item.view = self;
    }
    return self;
}

- (void)mouseDown:(NSEvent*)event
{
    _highlighted = !_highlighted;
    [self setNeedsDisplay:YES];
}

- (void)setHighlighted:(BOOL)newFlag
{
    if (_highlighted == newFlag) return;
    _highlighted = newFlag;
    [self setNeedsDisplay:YES];
}

//
//- (id)initWithFrame:(NSRect)frame
//{
//    self = [super initWithFrame:frame];
//    if (self) {
//        // Initialization code here.
//    }
//    
//    return self;
//}
//
- (void)drawRect:(NSRect)dirtyRect
{
    [_status_item drawStatusBarBackgroundInRect:dirtyRect withHighlight:_highlighted];
    if (_highlighted)
        [[NSColor highlightColor] set];
    else
        [[NSColor clearColor] set];
    NSImage *icon = _image;//_highlighted ? _alternate_image : _image;
    NSSize iconSize = [icon size];
    NSRect bounds = self.bounds;
    CGFloat iconX = roundf((NSWidth(bounds) - iconSize.width) / 2);
    CGFloat iconY = roundf((NSHeight(bounds) - iconSize.height) / 2);
    NSPoint iconPoint = NSMakePoint(iconX, iconY);
    
	[icon drawAtPoint:iconPoint fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
}

- (NSRect)absoluteFrame
{
    NSRect frame = [self frame];
    frame.origin = [self.window convertBaseToScreen:frame.origin];
    return frame;
}

@end
