//
//  FITableCellDictKey.m
//  HelperTools
//
//  Created by Charles Guillot on 28/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FITableCellDictKey.h"
#import <objc/objc-class.h>

@implementation FITableCellDictKey

@synthesize rowIndex;
@synthesize columnIdentifier;

static NSMutableSet *items = nil;
static NSString *kTableCellDictKey = @"tableCellDictKey";

+ (id)tableCellDictKeyWithColumnIdentifer:(id)arg1 rowIndex:(long long)arg2
{
    
    if (items == nil) {
        items = [[NSMutableSet alloc] init];
    }
    id item = [[FITableCellDictKey alloc] initWithColumnIdentifier:arg1 rowIndex:arg2];
    id memberItem = [items member:item];
    if (memberItem == nil)
    {
        [items addObject:item];
        objc_setAssociatedObject(arg1, &kTableCellDictKey, item, OBJC_ASSOCIATION_RETAIN);
    }
    else
    {
        item = memberItem;
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

-(void)dealloc
{
    if ([items containsObject:self]){
        [items removeObject:self];
    }
    [super dealloc];
}

- (unsigned long long)hash
{
    return rowIndex ^ [columnIdentifier hash];
}

- (BOOL)hasSameColumnIdentifier:(id)arg1 andRowIndex:(long long)arg2
{
    // has the same row index.
    if (rowIndex != arg2)
        return NO;
    // Is a NSTableColumn.
    if ( [columnIdentifier isKindOfClass:[NSTableColumn class]] && [arg1 isKindOfClass:[NSTableColumn class]] ) {
        // Is equal.
        if (![columnIdentifier isEqual:arg1])
            return NO;
    }
    else {
        return NO;
    }
    
    return YES;
}

- (BOOL)isEqual:(id)arg1
{
    if (arg1 == self)
        return YES;
    if (!arg1 || ![arg1 isKindOfClass:[self class]])
        return NO;
    return [self hasSameColumnIdentifier:[arg1 columnIdentifier] andRowIndex:[arg1 rowIndex]];
}

@end