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
    NSURL* url = [self.dictKey getPath];
    
    // Get status by path
    sleep(1);
    // Set status
    [self.dictKey setNodeStatus:FINodeStatusSynced];
    
    [self.dictKey refreshCell];
    
    return;
}

@end
