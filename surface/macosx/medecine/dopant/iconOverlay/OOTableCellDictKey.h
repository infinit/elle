//
//  OOTableCellDictKey.h
//  Dopant
//
//  Created by Charles Guillot on 28/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OOCellDictKey-Protocol.h"
#import "OOCellDictKey.h"
#import "OOIconOverlay.h"

/**
 This class help to store a column cell in a dictionary
 */
@interface OOTableCellDictKey : OOCellDictKey
{
    /**
     The row index
     */
    long long           rowIndex;
    
    /**
     The column identifier
     */
    NSTableColumn       *columnIdentifier;
    
    /**
     The linked node
     */
    TFENode             *node;
}

@property long long     rowIndex;
@property(assign) id    columnIdentifier;
@property TFENode       *node;

/**
 Multiton instance getter method
 @param id a column identifier
 @param long long the row
 @param id the linked node
 @returns the multiton instance
 */
+ (id)tableCellDictKeyWithColumnIdentifer:(id)arg1 rowIndex:(long long)arg2 forNode:(id)arg3;

/**
 Multiton instance getter method
 @param id a column identifier
 @param long long the row
 @param id the linked node
 @returns the multiton instance
 */
- (id)initWithColumnIdentifier:(id)arg1 rowIndex:(long long)arg2 forNode:(id)arg3;

/**
 Help the isEqual method comparing columns and rows
 @param id a column identifier
 @param long long the row
 @returns YES if the column and he row are the same
 */
- (BOOL)hasSameColumnIdentifier:(id)arg1 andRowIndex:(long long)arg2;

@end