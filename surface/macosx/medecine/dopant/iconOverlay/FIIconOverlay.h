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
    FINodeStatusUnknowned,
    FINodeStatusSyncing,
    FINodeStatusSynced,
    FINodeStatusDisconected
} FINodeStatus;

@interface FIIconOverlay : NSObject
{
    NSOperationQueue        *nodeStatusOperationQueue;
    struct OpaqueIconRef    *syncingIconRef;
    struct OpaqueIconRef    *syncedIconRef;
    struct OpaqueIconRef    *disconnectedIconRef;
}

@property(readonly) NSOperationQueue        *nodeStatusOperationQueue;
@property(readonly) struct OpaqueIconRef    *syncingIconRef;
@property(readonly) struct OpaqueIconRef    *syncedIconRef;
@property(readonly) struct OpaqueIconRef    *disconnectedIconRef;

@property(retain) NSMutableDictionary *nodesStatusDict;

+ (FIIconOverlay*) instance;

- (IconRef) iconRefWithCell:(FINodeStatus)arg1;
- (IconRef)iconRefForURL:(NSURL *)arg1;
- (void) addStatusOpperationToQueue:(id)arg1;

@end
