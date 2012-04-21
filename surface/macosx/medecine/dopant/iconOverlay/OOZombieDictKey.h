//
//  OOZombieDictKey.h
//  Dopant
//
//  Created by Charles Guillot on 02/04/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OOCellDictKey-Protocol.h"

/**
 This class is instanciated by cell dictionary keys
 in order to simulate an override of the cell's parrent
 dealloc method
 */
@interface OOZombieDictKey : NSObject
{
    /**
     The siblings cells
     */
    NSMutableArray  *linkedDictKeys;
    
    /**
     The parent cell
     */
    id              parent;
}

@property(retain) NSMutableArray *linkedDictKeys;
@property(assign) id parent;

/**
 Add a cell dictionary key
 @param id a cell dictionary key
 */
- (void) addDictKey:(id)arg1;

@end
