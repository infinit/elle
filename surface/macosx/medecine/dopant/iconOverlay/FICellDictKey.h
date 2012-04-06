//
//  FITableCellDictKey.h
//  HelperTools
//
//  Created by Charles Guillot on 28/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "FICellDictKey-Protocol.h"
#import "FIIconOverlay.h"

@interface FICellDictKey : NSOperation <FICellDictKey_Protocol>
{
    FINodeStatus    nodeStatus;
}

@property FINodeStatus  nodeStatus;

- (void) refreshCell;
- (NSURL *) getPath;
- (unsigned long long) hash;
- (BOOL) isEqual:(id)arg1;
- (void) cleanUp;

@end

@interface NSObject (FICellDictKey)

- (void)runAtDealloc:(id)arg1;

@end