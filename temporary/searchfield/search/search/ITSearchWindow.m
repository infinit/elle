//
//  ITSearchWindow.m
//  search
//
//  Created by infinit on 12/5/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "ITSearchWindow.h"

@implementation ITSearchWindow

@synthesize hidden = _hidden;

- (id)initWithContentRect:(NSRect)contentRect
                styleMask:(NSUInteger)aStyle
                  backing:(NSBackingStoreType)bufferingType
                    defer:(BOOL)flag
{
    if ((self = [super initWithContentRect:contentRect
								 styleMask:NSBorderlessWindowMask
								   backing:bufferingType
									 defer:flag]))
    {
		[self setOpaque:YES];
		[self setBackgroundColor:[NSColor clearColor]];
		[self setLevel:NSPopUpMenuWindowLevel+1];
        [self setHidesOnDeactivate:YES];
        _hidden = YES;
    }
    
    return self;
}

- (void)showWithPosition:(NSRect)pos
{
    [self setFrame:NSMakeRect(pos.origin.x, pos.origin.y - pos.size.height, pos.size.width, pos.size.height) display:YES];
    if (_hidden)
    {
        [self setAlphaValue:0.9];
        [self setIsVisible:YES];
        [self makeKeyAndOrderFront:self];
        [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
        _hidden = NO;
    }
}

- (void)updatePosition:(NSRect)pos
{
    [self setFrame:NSMakeRect(pos.origin.x, pos.origin.y - pos.size.height, pos.size.width, pos.size.height) display:_hidden];
}

- (void) hide
{
    [self setIsVisible:NO];
    _hidden = YES;
}

- (BOOL)canBecomeKeyWindow { return NO; }
- (BOOL)canBecomeMainWindow { return NO; }
- (BOOL)acceptsFirstResponder { return NO; }
- (BOOL)becomeFirstResponder { return NO; }
- (BOOL)resignFirstResponder { return YES; }


@end
