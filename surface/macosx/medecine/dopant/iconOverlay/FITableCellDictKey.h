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

@interface FITableCellDictKey : NSOperation <FICellDictKey_Protocol>
{
    long long       rowIndex;
    NSTableColumn   *columnIdentifier;
    TFENode         *node;
    FINodeStatus    nodeStatus;
}
@property long long     rowIndex;
@property(assign) id    columnIdentifier;
@property TFENode       *node;
@property FINodeStatus  nodeStatus;

+ (id)tableCellDictKeyWithColumnIdentifer:(id)arg1 rowIndex:(long long)arg2 forNode:(id)arg3;
- (id)initWithColumnIdentifier:(id)arg1 rowIndex:(long long)arg2 forNode:(id)arg3;
- (unsigned long long)hash;
- (BOOL)hasSameColumnIdentifier:(id)arg1 andRowIndex:(long long)arg2;
- (BOOL)isEqual:(id)arg1;
@end
