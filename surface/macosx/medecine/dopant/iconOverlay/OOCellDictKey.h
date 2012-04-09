//
//  FITableCellDictKey.h
//  HelperTools
//
//  Created by Charles Guillot on 28/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OOCellDictKey-Protocol.h"
#import "OOIconOverlay.h"

@interface OOCellDictKey : NSOperation <OOCellDictKey_Protocol>
{
    OONodeStatus    nodeStatus;
}

@property OONodeStatus  nodeStatus;

- (void) refreshCell;
- (NSURL *) getPath;
- (unsigned long long) hash;
- (BOOL) isEqual:(id)arg1;
- (void) cleanUp;

@end

@interface NSObject (FICellDictKey)

- (void)runAtDealloc:(id)arg1;

@end