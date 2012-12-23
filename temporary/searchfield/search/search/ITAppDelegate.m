//
//  ITAppDelegate.m
//  search
//
//  Created by infinit on 12/5/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "ITAppDelegate.h"

@implementation ITAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
}

- (void) awakeFromNib
{
    NSLog(@"awake from nib: %@", self.search_field);
    [self.window setDelegate:self];
    [self.search_field.cell setSendsWholeSearchString:NO];
    [self.search_field.cell setTarget:self];
    [self.search_field.cell setAction:@selector(updateFilter:)];
    self.search_window = [[ITSearchWindow alloc] initWithParent:self.window];
    
    // Initialize with the right position, further updates are made by windowDidMove event.
    [self.search_window updatePosition:[self _getWindowPosition]];
}

- (void)updateFilter:(id)sender
{
    NSLog(@"search for %@", [self.search_field.cell stringValue]);
    NSString* str = [self.search_field.cell stringValue];
    if (str && [str length])
        [self.search_window show];
    else
        [self.search_window hide];
}

- (NSRect)_getWindowPosition
{
    NSRect pos;
    pos.origin = self.window.frame.origin;
    pos.origin.x += self.search_field.frame.origin.x;
    pos.origin.y += self.search_field.frame.origin.y;
    pos.size = self.search_field.frame.size;
    pos.size.height = 200;
    return pos;
}

-(void)windowDidMove:(NSNotification*)notification
{
    assert(self.window == notification.object);
    [self.search_window updatePosition:[self _getWindowPosition]];
}

@end
