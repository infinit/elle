//
//  FITableCellDictKey.h
//  HelperTools
//
//  Created by Charles Guillot on 28/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface FITableCellDictKey : NSObject
{
    long long rowIndex;
    id columnIdentifier;
}
@property long long rowIndex;
@property(retain) id columnIdentifier;

+ (id)tableCellDictKeyWithColumnIdentifer:(id)arg1 rowIndex:(long long)arg2;
- (id)initWithColumnIdentifier:(id)arg1 rowIndex:(long long)arg2;
- (unsigned long long)hash;
- (BOOL)hasSameColumnIdentifier:(id)arg1 andRowIndex:(long long)arg2;
- (BOOL)isEqual:(id)arg1;
@end
