//
//  ITSearchResults.m
//  search
//
//  Created by Patrick Perlmutter on 12/6/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "ITSearchResults.h"

@implementation ITSearchResults

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
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
