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

- (IASearchResultsTableView*)table_view
{
    return _table_view;
}

- (id)initWithParent:(NSWindow*)parent
{
    _table_view = nil;

    if (self)
    {
        _parent = parent;
        _table_view = nil;
    }
    return (self = [super init]); // This will call initWithContentRect ...
}

- (id)initWithContentRect:(NSRect)contentRect
                styleMask:(NSUInteger)aStyle
                  backing:(NSBackingStoreType)bufferingType
                    defer:(BOOL)flag
{
    if ((self = [super initWithContentRect:contentRect
								 styleMask:NSBorderlessWindowMask
								   backing:bufferingType
									 defer:NO]))
    {
		[self setBackgroundColor:[NSColor greenColor]];
		[self setLevel:NSPopUpMenuWindowLevel+1];
        [self setHidesOnDeactivate:YES];
        [self setOpaque:YES];
        _hidden = YES;
        
        NSScrollView* container = [[NSScrollView alloc] initWithFrame:contentRect];
        _table_view = [[IASearchResultsTableView alloc] initWithFrame:contentRect];
        NSTableColumn* column = [[NSTableColumn alloc] initWithIdentifier:@"SearchUserResult"];
        [column setWidth:200];
        [_table_view addTableColumn:column];
        [_table_view setAllowsMultipleSelection:NO];
        [_table_view setAllowsEmptySelection:NO];
        [_table_view setHeaderView:nil];
        [_table_view setRowHeight:20];
        [container setDocumentView:_table_view];
        [container setHasVerticalScroller:YES];
        [container setAutohidesScrollers:YES];
        [self setContentView:container];
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
    
  //  [_table_view setFrame:[NSWindow contentRectForFrameRect:frame
  //                                                styleMask:NSBorderlessWindowMask]];
//    [_table_view setNeedsDisplay:!_hidden];
//    [_table_view setNeedsLayout:!_hidden];
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
