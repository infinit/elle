//
//  OOIconOverlay.h
//  Dopant
//
//  Created by Charles Guillot on 27/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OOSwizzle.h"
#import "CDStructures.h"

/**
 This enum allow to store a node status
 */
typedef enum {
    OONodeStatusUnknowned,
    OONodeStatusSyncing,
    OONodeStatusSynced,
    OONodeStatusDisconected
} OONodeStatus;

/**
 This class swizzle all needed icon overlay methods
 */
@interface OOIconOverlay : NSObject
{
    /**
     The operation queue
     */
    NSOperationQueue                        *nodeStatusOperationQueue;
    
    /**
     The icon ref for the syncing status
     */
    struct OpaqueIconRef                    *syncingIconRef;
    /**
     The icon ref for the synced status
     */
    struct OpaqueIconRef                    *syncedIconRef;
    /**
     The icon ref for the disconnected status
     */
    struct OpaqueIconRef                    *disconnectedIconRef;
    
    /**
     The image for the syncing status
     */
    NSImage                                 *syncingImage;
    /**
     The image for the synced status
     */
    NSImage                                 *syncedImage;
    /**
     The image for the disconnected status
     */
    NSImage                                 *disconnectedImage;
}

@property(readonly) NSOperationQueue        *nodeStatusOperationQueue;

@property(readonly) struct OpaqueIconRef    *syncingIconRef;
@property(readonly) struct OpaqueIconRef    *syncedIconRef;
@property(readonly) struct OpaqueIconRef    *disconnectedIconRef;

@property(readonly) NSImage                 *syncingImage;
@property(readonly) NSImage                 *syncedImage;
@property(readonly) NSImage                 *disconnectedImage;

/**
 Get the singleton instance
 @returns the instance
 */
+ (OOIconOverlay*) instance;

/**
 Get the icon ref associated to a node status
 @param nodeStatus the node status
 @returns the icon ref
 */
- (IconRef) iconRefWithNodeStatus:(OONodeStatus)arg1;

/**
 Get the image associated to a node status
 @param nodeStatus the node status
 @returns the image
 */
- (NSImage *) imageWithNodeStatus:(OONodeStatus)arg1;

/**
 Convert an URL to an icon ref
 @param URL the URL
 @returns the icon ref
 */
- (IconRef) iconRefForURL:(NSURL *)arg1;

/**
 Add an operation to the operation queue
 @param operation the operation
 */
- (void) addStatusOpperationToQueue:(id)arg1;

@end
