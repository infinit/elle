//
//  OOBrowserWindow.m
//  FinderPanel
//
//  Created by Charles Guillot on 10/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOBrowserWindow.h"

@implementation OOBrowserWindow

@synthesize isFullHeight;

- (id)init
{
    self = [super init];
    if (self) {
        self.isFullHeight = YES;
    }
    
    return self;
}
@end
