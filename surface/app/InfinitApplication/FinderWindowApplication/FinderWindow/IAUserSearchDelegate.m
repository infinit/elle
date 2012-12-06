//
//  IAUserSearchBarDelegate.m
//  FinderWindow
//
//  Created by Patrick Perlmutter on 12/6/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAUserSearchDelegate.h"

@implementation IAUserSearchDelegate

- (void)awakeFromNib
{
    NSLog(@"Awake from nib, cell: %@, window: %@", self.search_bar.cell, self.main_window);
    [self.search_bar.cell setSendsWholeSearchString:NO];
    [self.search_bar.cell setTarget:self];
    [self.search_bar.cell setAction:@selector(updateFilter:)];
    self.search_window = [[IASearchResultsWindow alloc] initWithParent:self.main_window];
    [self.search_window updatePosition:[self _getWindowPosition]];
}
- (NSRect)_getWindowPosition
{
    NSRect pos;
    pos.origin = self.main_window.frame.origin;
    pos.origin.x += self.search_bar.frame.origin.x;
    pos.origin.y += self.search_bar.frame.origin.y;
    pos.size = self.search_bar.frame.size;
    pos.size.height = 200;
    return pos;
}

- (void)updateFilter:(id)sender
{

    NSLog(@"search for %@", [self.search_bar.cell stringValue]);
    NSString* str = [self.search_bar.cell stringValue];

    [self.main_view_controller refresh];
    if (str && [str length])
    {
        [self.search_window updatePosition:[self _getWindowPosition]];
        [self.search_window show];
    }
    else
        [self.search_window hide];
}

@end
