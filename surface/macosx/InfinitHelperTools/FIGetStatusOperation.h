//
//  FIGetStatusOperationQueue.h
//  HelperTools
//
//  Created by Charles Guillot on 29/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CDStructures.h"
#import "FIIconOverlay.h"

@interface FIGetStatusOperation : NSOperation
{
    id dictKey;
}
@property(retain) id dictKey;

- (id) initWithDictKey:(id)arg1;
- (NodeStatus) getStatusByPath:(NSURL *)arg1;
- (TFENode *)GetNode;
@end
