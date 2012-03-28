//
//  FIIconOverlay.h
//  HelperTools
//
//  Created by Charles Guillot on 27/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "FISwizzle.h"
#import "CDStructures.h"

typedef enum {
    Syncing,
    Synced,
    Disconected
} NodeStatus;

@interface FIIconOverlay : NSObject
{
    NSMutableDictionary     *nodesStatusDict; // fake ivar
    NSOperationQueue        *nodeStatusOperationQueue;
    struct OpaqueIconRef    *syncingIconRef;
    struct OpaqueIconRef    *syncedIconRef;
    struct OpaqueIconRef    *disconnectedIconRef;
}

@property(readonly) struct OpaqueIconRef *syncingIconRef;
@property(readonly) struct OpaqueIconRef *syncedIconRef;
@property(readonly) struct OpaqueIconRef *disconnectedIconRef;

+ (FIIconOverlay*) instance;

- (IconRef)iconRefForURL:(NSURL *)arg1;
- (id) CellStatusWithCell:(id)arg1;
- (void) removeCellStatusWithCell:(id)arg1;
- (void) addCellStatusWithCell:(id)arg1;

@end
