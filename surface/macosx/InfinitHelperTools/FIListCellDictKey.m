//
//  FIListCellDictKey.m
//  HelperTools
//
//  Created by Charles Guillot on 30/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FIListCellDictKey.h"

@implementation FIListCellDictKey

@synthesize node;

- (NSURL *) getPath
{
    if ([self.node respondsToSelector:@selector(node)]) {
        // TODO return PATH
        return [self.node node];
    }
    else {
        return nil;
    }
}

- (void) refreshCell
{
    // TODO refresh cell
    return;
}

@end
