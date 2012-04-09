//
//  FIListCellDictKey.h
//  HelperTools
//
//  Created by Charles Guillot on 30/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "FICellDictKey-Protocol.h"
#import "FICellDictKey.h"
#import "FIIconOverlay.h"

@interface FIIconCellDictKey : FICellDictKey
{
    id              cell;
}

@property(assign) id    cell;

+ (id) iconCellDictKeyWithCell:(id)arg1;

- (id) initWithCell:(id)arg1;


@end
