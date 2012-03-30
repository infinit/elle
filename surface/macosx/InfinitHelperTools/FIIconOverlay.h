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
    FISyncing,
    FISynced,
    FIDisconected
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

@property(retain) NSMutableDictionary *nodesStatusDict;

+ (FIIconOverlay*) instance;

- (IconRef) iconRefWithCell:(id)arg1;
- (IconRef)iconRefForURL:(NSURL *)arg1;
- (id) CellStatusWithCell:(id)arg1;
- (void) removeCellStatusWithCell:(id)arg1;
- (void) addCellStatusWithCell:(id)arg1;

@end
