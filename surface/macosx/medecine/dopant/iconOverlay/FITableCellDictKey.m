//
//  FITableCellDictKey.m
//  HelperTools
//
//  Created by Charles Guillot on 28/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FITableCellDictKey.h"
#import <objc/objc-class.h>
#import "ZombieDictKey.h"

@implementation FITableCellDictKey

@synthesize rowIndex;
@synthesize columnIdentifier;
@synthesize node;

static NSMutableSet *items = nil;

+ (id)tableCellDictKeyWithColumnIdentifer:(id)arg1 rowIndex:(long long)arg2 forNode:(id)arg3
{
    @synchronized(items)
    {
        if (items == nil) {
            items = [[NSMutableSet alloc] init];
        }
        
        FITableCellDictKey *matchItem = nil;
        for (FITableCellDictKey *item in [items allObjects]) {
            if (item.columnIdentifier == arg1 && item.rowIndex == arg2) {
                matchItem = item;
                break;
            }
        }
        
        if (matchItem == nil)
        {
            matchItem = [[FITableCellDictKey alloc] initWithColumnIdentifier:arg1 rowIndex:arg2  forNode:arg3];
            [items addObject:matchItem];
            [arg1 runAtDealloc:matchItem];
            [matchItem autorelease];
        }
        
        return matchItem;
    }
}

- (id)initWithColumnIdentifier:(id)arg1 rowIndex:(long long)arg2 forNode:(id)arg3
{
    self = [super init];
    
    self.columnIdentifier = arg1;
    self.rowIndex = arg2;
    self.node = (TFENode *)arg3;
    self.nodeStatus = FINodeStatusUnknowned;
    
    [[FIIconOverlay instance] addStatusOpperationToQueue:self];
    
    return self;
}

-(void)dealloc
{
    [super dealloc];
}

- (unsigned long long)hash
{
    if (self.columnIdentifier == nil)
        return 0;
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
    if (self.columnIdentifier == nil || ((FITableCellDictKey *)arg1).columnIdentifier == nil)
        return NO;
    
    return [self hasSameColumnIdentifier:((FITableCellDictKey *)arg1).columnIdentifier andRowIndex:((FITableCellDictKey *)arg1).rowIndex];
}

- (NSURL *) getPath
{
    NSURL *path = [NSURL fileURLWithPath:
                   [[NSClassFromString(@"NSNavFBENode") _nodeWithFBENode:
                     ((TFENode *)self.node)->fNodeRef] path]];
    
    return [path autorelease];
}

- (void) refreshCell
{
    // TODO refresh
    return;
}

- (void) cleanUp
{
    self.rowIndex = -1;
    self.columnIdentifier = nil;
    self.node = NULL;
    @synchronized(items)
    {
        if ([items containsObject:self]){
            [items removeObject:self];
        }
    }
    [self cancel];
    
}

@end