//
//  OOListCellDictKey.h
//  Dopant
//
//  Created by Charles Guillot on 30/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OOCellDictKey-Protocol.h"
#import "OOCellDictKey.h"
#import "OOIconOverlay.h"

/**
 This class help to store a list cell in a dictionary
 */
@interface OOListCellDictKey : OOCellDictKey
{    
    /**
     The linked cell
     */
    id                  cell;
}

@property(assign) id    cell;

/**
 Multiton instance getter method
 @param id a cell
 @returns the multiton instance
 */
+ (id) listCellDictKeyWithCell:(id)arg1;

/**
 Init with cell
 @param id a cell
 @returns the new instance
 */
- (id) initWithCell:(id)arg1;


@end
