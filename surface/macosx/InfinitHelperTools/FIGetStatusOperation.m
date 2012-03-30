//
//  FIGetStatusOperationQueue.m
//  HelperTools
//
//  Created by Charles Guillot on 29/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FIGetStatusOperation.h"
#import "FITableCellDictKey.h"

@implementation FIGetStatusOperation

@synthesize dictKey;

- (id) initWithDictKey:(id)arg1
{
    self = [self init];
    self.dictKey = arg1;
    // TODO : add dependency
    return self;
}

- (void) main
{
    if ([self.dictKey isKindOfClass:NSClassFromString(@"TListViewIconAndTextCell")])
    {
        if ([self.dictKey respondsToSelector:@selector(node)]) {
            TFENode *node = [self.dictKey node];
            // TODO get node status
            
            // Set new node status
            CFDictionarySetValue( (CFMutableDictionaryRef)[FIIconOverlay instance].nodesStatusDict, self.dictKey, [NSNumber numberWithInt:FISynced] );
            
            // Redraw cell
            [[self.dictKey controlView] updateCell:self.dictKey];
            
            // If is active schedule status getter.
        }
    }
    else if ([self.dictKey isKindOfClass:[FITableCellDictKey class]])
    {
        // TODO Columnview
    }
    return;
}

- (NodeStatus) getStatusByPath:(NSURL *)arg1
{
    return FISynced;
}

- (TFENode *)GetNode
{
    if ([self.dictKey isKindOfClass:NSClassFromString(@"TListViewIconAndTextCell")])
    {
        if ([self.dictKey respondsToSelector:@selector(node)]) {
            return [self.dictKey node];
        }
        else {
            return NULL;
        }
    }
    else if ([self.dictKey isKindOfClass:[FITableCellDictKey class]])
    {

    }
    else {
        return NULL;
    }
}



@end
