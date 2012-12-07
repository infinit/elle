//
//  IASearchResultsWindow.m
//  FinderWindow
//
//  Created by Patrick Perlmutter on 12/6/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IASearchResultsWindow.h"
#import "IASearchResultsTableView.h"

@implementation IASearchResultsWindow
{
@private
    NSWindow*                   _parent;
    IASearchResultsTableView*   _table_view;
}

@synthesize hidden = _hidden;

- (id)initWithParent:(NSWindow*)parent
        andTableView:(IASearchResultsTableView*)table_view;
{
    self = [super init];
    if (self)
    {
        _parent = parent;
        _table_view = table_view;
        NSLog(@"init window with tableview %@", table_view);
    }
    return self;
}

- (id)initWithContentRect:(NSRect)contentRect
                styleMask:(NSUInteger)aStyle
                  backing:(NSBackingStoreType)bufferingType
                    defer:(BOOL)flag
{
    if ((self = [super initWithContentRect:contentRect
								 styleMask:aStyle//NSBorderlessWindowMask
								   backing:bufferingType
									 defer:flag]))
    {
		[self setBackgroundColor:[NSColor greenColor]];
		[self setLevel:NSPopUpMenuWindowLevel+1];
        [self setHidesOnDeactivate:YES];
        [self setOpaque:YES];
        _hidden = YES;
        _table_view = [[IASearchResultsTableView alloc] initWithFrame:contentRect];
      //  [_table_view setFrame:NSMakeRect(0, 0, 200, 200)];
        [self setContentView:_table_view.enclosingScrollView];
        [_table_view setHidden:NO];
       // [_table_view setBackgroundColor:[NSColor orangeColor]];
    }
    
    return self;
}


- (void)updatePosition:(NSRect)pos
{
    NSRect frame = NSMakeRect(pos.origin.x,
                              pos.origin.y - pos.size.height,
                              pos.size.width,
                              pos.size.height);
    [self setFrame:frame
           display:_hidden];
   // [_table_view.enclosingScrollView setFrame:NSMakeRect(10, 10, 100, 100)];//[NSWindow contentRectForFrameRect:frame
                               //                   styleMask:NSBorderlessWindowMask]];
    [_table_view setNeedsDisplay:!_hidden];
    [_table_view setNeedsLayout:!_hidden];
}

- (void) hide
{
    [self orderOut:nil];
    [_parent removeChildWindow:self];
    _hidden = YES;
}

- (void)show
{
    [self setIsVisible:YES];
    [self setAlphaValue:0.9];
    [self makeKeyAndOrderFront:self];
    [self setLevel:NSPopUpMenuWindowLevel+1];
    [_table_view setNeedsDisplay:YES];
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
