//
//  FIListCellDictKey.h
//  HelperTools
//
//  Created by Charles Guillot on 30/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OOCellDictKey-Protocol.h"
#import "OOCellDictKey.h"
#import "OOIconOverlay.h"

@interface OOIconCellDictKey : OOCellDictKey
{
    id              cell;
}

@property(assign) id    cell;

+ (id) iconCellDictKeyWithCell:(id)arg1;

- (id) initWithCell:(id)arg1;


@end
