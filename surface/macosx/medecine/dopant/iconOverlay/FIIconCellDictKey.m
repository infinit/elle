//
//  FIListCellDictKey.m
//  HelperTools
//
//  Created by Charles Guillot on 30/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FIIconCellDictKey.h"
#import <objc/objc-class.h>
#import "ZombieDictKey.h"

@implementation FIIconCellDictKey

@synthesize cell;

static NSMutableSet *items = nil;

+ (id)iconCellDictKeyWithCell:(id)arg1
{
    @synchronized(items)
    {
        if (items == nil) {
            items = [[NSMutableSet alloc] init];
        }
        FIIconCellDictKey *matchItem = nil;
        for (FIIconCellDictKey *item in [items allObjects]) {
            if (item.cell != nil) {
                if (item.cell == arg1) {
                    matchItem = item;
                    break;
                }
            }
        }
        
        if (matchItem == nil)
        {
            matchItem = [[FIIconCellDictKey alloc] initWithCell:arg1];
            [items addObject:matchItem];
            [arg1 runAtDealloc:matchItem];
            
            [matchItem release];
        }
        
        return matchItem;
    }
}

- (id) initWithCell:(id)arg1
{
    self = [super init];
    
    self.cell = arg1; // TODO add release link
    self.nodeStatus = FINodeStatusUnknowned;
    
    [[FIIconOverlay instance] addStatusOpperationToQueue:self];
    
    return self;
}
- (unsigned long long)hash
{
    if (self.cell == nil)
        return 0;
    return [self.cell hash];
}
- (BOOL)isEqual:(id)arg1
{
    if (arg1 == self)
        return YES;
    if (!arg1 || ![arg1 isKindOfClass:[self class]])
        return NO;
    if (self.cell == nil || ((FIIconCellDictKey *)arg1).cell == nil)
        return NO;
    return self.cell == ((FIIconCellDictKey *)arg1).cell;
}

- (NSURL *) getPath
{
    id finode = [self.cell representedItem];
    
    NSURL *path = [NSURL fileURLWithPath:
                   [[NSClassFromString(@"NSNavFBENode") _nodeWithFBENode:
                     [finode nodeRef]] path]];
    
    return path;
}

- (void) refreshCell
{   
    [NSException raise:NSInternalInconsistencyException 
                format:@"You must override %@ in a subclass", NSStringFromSelector(_cmd)];
}

- (void) cleanUp
{
    self.cell = nil;
    @synchronized(items)
    {
        if ([items containsObject:self]){
            [items removeObject:self];
        }
    }
    [self cancel];
    
}

@end
