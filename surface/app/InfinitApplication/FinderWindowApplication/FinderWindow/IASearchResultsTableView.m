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
    NSLog(@"TABLEVIEW awake from nib");    
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
        [self setBackgroundColor:[NSColor orangeColor]];
    }
    
    return self;
}
//
//- (void)drawRect:(NSRect)dirtyRect
//{
//    // Drawing code here.
//}

@end
