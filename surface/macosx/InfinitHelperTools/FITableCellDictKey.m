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
static NSString *kTableCellDictKeyArray = @"fi_TableCellDictKeyArray";

+ (id)tableCellDictKeyWithColumnIdentifer:(id)arg1 rowIndex:(long long)arg2
{
    @synchronized(items)
    {
        if (items == nil) {
            items = [[NSMutableSet alloc] init];
        }
        id item = [[FITableCellDictKey alloc] initWithColumnIdentifier:arg1 rowIndex:arg2];
        id memberItem = [items member:item];
        if (memberItem == nil)
        {
            [items addObject:item];
            [item autorelease];
            return item;
        }
        else
        {
            [item release];
            return memberItem;
        }
    }
}

- (id)initWithColumnIdentifier:(id)arg1 rowIndex:(long long)arg2
{
    self = [super init];
    
    self.columnIdentifier = arg1;
    self.rowIndex = arg2;
    
    return self;
}

-(void)dealloc
{
    if ([items containsObject:self]){
        [items removeObject:self];
    }
    [columnIdentifier release];
    [super dealloc];
}

- (unsigned long long)hash
{
    return self.rowIndex ^ [self.columnIdentifier hash];
}

- (BOOL)hasSameColumnIdentifier:(id)arg1 andRowIndex:(long long)arg2
{
    // has the same row index.
    if (self.rowIndex != arg2)
        return NO;
    // Is a NSTableColumn.
    if ( [self.columnIdentifier isKindOfClass:[NSTableColumn class]] && [arg1 isKindOfClass:[NSTableColumn class]] ) {
        // Is equal.
        if (![self.columnIdentifier isEqual:arg1])
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
    return [self hasSameColumnIdentifier:((FITableCellDictKey *)arg1).columnIdentifier andRowIndex:((FITableCellDictKey *)arg1).rowIndex];
}

@end