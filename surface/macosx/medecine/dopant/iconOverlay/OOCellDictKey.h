//
//  OOCellDictKey.h
//  Dopant
//
//  Created by Charles Guillot on 28/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OOCellDictKey-Protocol.h"
#import "OOIconOverlay.h"

/**
 This class help to store a cell in a dictionary
 */
@interface OOCellDictKey : NSOperation <OOCellDictKey_Protocol>
{
    /**
     The status of the node
     */
    OONodeStatus        nodeStatus;
}

@property OONodeStatus  nodeStatus;
/**
 Refresh a cell when needed
 */
- (void) refreshCell;
/**
 Get the path linked to the cell
 @return the linked path
 */
- (NSURL *) getPath;
/**
 Override NSObject hash in order to avoid duplicates
 */
- (unsigned long long) hash;
/**
 Override NSObject isEqual in order to test equaliy
 @param id the cell to compare with
 @returns YES if is equal otherwise NO
 */
- (BOOL) isEqual:(id)arg1;
/**
 Clean up all retained & retaining dependancies
 */
- (void) cleanUp;

@end
/**
 This class simulate a dealloc override of the parent
 */
@interface NSObject (FICellDictKey)

/**
 Run when cell's parent is deallocated.
 @param id the linked cell
 */
- (void)runAtDealloc:(id)arg1;

@end