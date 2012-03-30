//
//  FITableCellDictKey.h
//  HelperTools
//
//  Created by Charles Guillot on 28/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "FICellDictKey-Protocol.h"

@interface FITableCellDictKey : NSObject <FICellDictKey_Protocol>
{
    long long rowIndex;
    NSTableColumn *columnIdentifier;
    id node;
}
@property long long rowIndex;
@property(nonatomic, retain) id columnIdentifier;
@property(nonatomic, retain) id node;

+ (id)tableCellDictKeyWithColumnIdentifer:(id)arg1 rowIndex:(long long)arg2 forNode:(id)arg3;
- (id)initWithColumnIdentifier:(id)arg1 rowIndex:(long long)arg2 forNode:(id)arg3;
- (unsigned long long)hash;
- (BOOL)hasSameColumnIdentifier:(id)arg1 andRowIndex:(long long)arg2;
- (BOOL)isEqual:(id)arg1;
@end
