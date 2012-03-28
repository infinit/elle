//
//  FITableCellDictKey.m
//  HelperTools
//
//  Created by Charles Guillot on 28/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FITableCellDictKey.h"

@implementation FITableCellDictKey

@synthesize rowIndex;
@synthesize columnIdentifier;

+ (id)tableCellDictKeyWithColumnIdentifer:(id)arg1 rowIndex:(long long)arg2
{
    static NSMutableDictionary *items = nil;
    
    if (items == nil) {
        items = [[NSMutableDictionary alloc] init];
    }
    
    unsigned long long tempHash = arg2 * [arg1 hash];
    id item = [items objectForKey:[NSNumber numberWithUnsignedLongLong:tempHash]];
    if ( item == nil)
    {
        item = [[FITableCellDictKey alloc] initWithColumnIdentifier:arg1 rowIndex:arg2];
        [items setObject:[NSNumber numberWithUnsignedLongLong:[item hash]] forKey:item];
    }
    
    return item;
}

- (id)initWithColumnIdentifier:(id)arg1 rowIndex:(long long)arg2
{
    self = [super init];
    
    columnIdentifier = arg1;
    rowIndex = arg2;
    
    return self;
}

- (unsigned long long)hash
{
    return rowIndex ^ [columnIdentifier hash];
}

- (BOOL)isEqualToDictKey:(FITableCellDictKey *)arg1
{
    if (self == arg1)
        return YES;
    
    if (rowIndex != [arg1 rowIndex])
        return NO;
    
    if (![columnIdentifier isEqual:[arg1 columnIdentifier]])
        return NO;
    
    return YES;
}

- (BOOL)isEqual:(id)arg1
{
    if (arg1 == self)
        return YES;
    if (!arg1 || ![arg1 isKindOfClass:[self class]])
        return NO;
    return [self isEqualToDictKey:arg1];
}

@end