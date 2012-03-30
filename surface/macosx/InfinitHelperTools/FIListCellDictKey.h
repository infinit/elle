//
//  FIListCellDictKey.h
//  HelperTools
//
//  Created by Charles Guillot on 30/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "FICellDictKey-Protocol.h"
#import "FIIconOverlay.h"

@interface FIListCellDictKey : NSObject <FICellDictKey_Protocol>
{
    id              cell;
    FINodeStatus    nodeStatus;
}

@property(nonatomic, retain) id cell;
@property FINodeStatus nodeStatus;

+ (id) listCellDictKeyWithCell:(id)arg1;

- (id) initWithCell:(id)arg1;


@end
