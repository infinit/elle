//
//  IASearchResultsTableView.m
//  FinderWindow
//
//  Created by Patrick Perlmutter on 12/6/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IASearchResultsTableView.h"

@implementation IASearchResultsTableView

- (void) awakeFromNib
{
    NSLog(@"TABLEVIEW awake from nib: %@", self);
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    NSLog(@"TABLEVIEW Initwithframe: %@", self);        
        [self setBackgroundColor:[NSColor orangeColor]];
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
    NSLog(@"SearchResults.drawRect:(%f,%f, %f, %f)",
          dirtyRect.origin.x,
          dirtyRect.origin.y,
          dirtyRect.size.width,
          dirtyRect.size.height);
}

@end
