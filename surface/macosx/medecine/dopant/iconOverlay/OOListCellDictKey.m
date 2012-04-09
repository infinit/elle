//
//  FIListCellDictKey.m
//  HelperTools
//
//  Created by Charles Guillot on 30/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOListCellDictKey.h"
#import <objc/objc-class.h>
#import "OOZombieDictKey.h"

@implementation OOListCellDictKey

@synthesize cell;

static NSMutableSet *items = nil;

+ (id)listCellDictKeyWithCell:(id)arg1
{
    @synchronized(items)
    {
        if (items == nil) {
            items = [[NSMutableSet alloc] init];
        }
        OOListCellDictKey *matchItem = nil;
        for (OOListCellDictKey *item in [items allObjects]) {
            if (item.cell != nil) {
                if (item.cell == arg1) {
                    matchItem = item;
                    break;
                }
            }
        }
        
        if (matchItem == nil)
        {
            matchItem = [[OOListCellDictKey alloc] initWithCell:arg1];
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
    self.nodeStatus = OONodeStatusUnknowned;
    
    [[OOIconOverlay instance] addStatusOpperationToQueue:self];
    
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
    if (self.cell == nil || ((OOListCellDictKey *)arg1).cell == nil)
        return NO;
    return self.cell == ((OOListCellDictKey *)arg1).cell;
}

- (NSURL *) getPath
{
    if (self.cell == nil)
        return nil;
    
    if ([self.cell respondsToSelector:@selector(node)]) {
        NSURL *path = [NSURL fileURLWithPath:[[NSClassFromString(@"NSNavFBENode") _nodeWithFBENode:((TFENode *)[self.cell node])->fNodeRef] path]];
        
        return [path autorelease];
    }
    else {
        return nil;
    }
}

- (void) refreshCell
{   
    if (self.cell == nil)
        return;
    
    if ([self.cell respondsToSelector:@selector(controlView)]) {
        [[self.cell controlView] updateCell:cell];
    }
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
