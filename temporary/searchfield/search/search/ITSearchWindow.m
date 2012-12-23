//
//  ITSearchWindow.m
//  search
//
//  Created by infinit on 12/5/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "ITSearchWindow.h"
#import "ITSearchResults.h"

@implementation ITSearchWindow
{
@private
    NSWindow*           _parent;
    ITSearchResults*    _table_view;
}

@synthesize hidden = _hidden;

- (id)initWithParent:(NSWindow*)parent
{
    self = [super init];
    if (self)
    {
        _parent = parent;
    }
    return self;
}

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
		[self setBackgroundColor:[NSColor clearColor]];
		[self setLevel:NSPopUpMenuWindowLevel+1];
        [self setHidesOnDeactivate:YES];
        [self setOpaque:NO];
        _hidden = YES;
        _table_view = [[ITSearchResults alloc] initWithFrame:contentRect];
        [self setContentView:_table_view];
    }
    
    return self;
}

- (void)showWithPosition:(NSRect)pos
{
    [self setFrame:NSMakeRect(pos.origin.x, pos.origin.y - pos.size.height, pos.size.width, pos.size.height) display:YES];
   // [self makeKeyAndOrderFront:self];
    [self orderWindow:NSWindowAbove relativeTo:[self.parentWindow windowNumber]];
    if (_hidden)
    {
        [self setOpaque:YES];
        [self setAlphaValue:0.9];
        [self setIsVisible:YES];
 //       [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
        _hidden = NO;
    }
}

- (void)updatePosition:(NSRect)pos
{
    [self setFrame:NSMakeRect(pos.origin.x,
                              pos.origin.y - pos.size.height,
                              pos.size.width,
                              pos.size.height)
           display:_hidden];
}

- (void) hide
{
    [self orderOut:nil];
    [_parent removeChildWindow:self];
//    [self setHidesOnDeactivate:YES];
//    [self setIsVisible:NO];
    _hidden = YES;
}

- (void)show
{
    [self setIsVisible:YES];
    [self setOpaque:YES];
    [self setAlphaValue:0.9];
    [self makeKeyAndOrderFront:self];
    [self setLevel:NSPopUpMenuWindowLevel+1];
    
    // This is needed, because after one call to orderOut:, the
    // child window stops to be positioned relatively to its parent...
    [ _parent addChildWindow:self ordered:NSWindowAbove];
    _hidden = NO;
}

- (BOOL)canBecomeKeyWindow { return NO; }
- (BOOL)canBecomeMainWindow { return NO; }
- (BOOL)acceptsFirstResponder { return NO; }
- (BOOL)becomeFirstResponder { return NO; }
- (BOOL)resignFirstResponder { return YES; }


@end
