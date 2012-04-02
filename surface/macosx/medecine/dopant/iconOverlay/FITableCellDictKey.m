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
@synthesize nodeStatus;

static NSMutableSet *items = nil;
static char overviewKey;

+ (id)tableCellDictKeyWithColumnIdentifer:(id)arg1 rowIndex:(long long)arg2 forNode:(id)arg3
{
    @synchronized(items)
    {
        if (items == nil) {
            items = [[NSMutableSet alloc] init];
        }
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"(columnIdentifier == %@) AND (rowIndex == %qi)", arg1, arg2];
        NSSet *filtered = [items filteredSetUsingPredicate:predicate];
        
        if ([filtered count] == 0)
        {
            id item = [[FITableCellDictKey alloc] initWithColumnIdentifier:arg1 rowIndex:arg2  forNode:arg3];
            ZombieDictKey *zombieReleaser = objc_getAssociatedObject(arg1, &overviewKey);
            
            if (zombieReleaser == nil) {
                zombieReleaser = [[ZombieDictKey alloc] init];
            }
            [zombieReleaser addDictKey:item];
            
            objc_setAssociatedObject ( arg1, &overviewKey, zombieReleaser, OBJC_ASSOCIATION_RETAIN );
            
            [items addObject:item];
            [item autorelease];
            return item;
        }
        else
        {
            return [filtered anyObject];
        }
    }
}

- (id)initWithColumnIdentifier:(id)arg1 rowIndex:(long long)arg2 forNode:(id)arg3
{
    self = [super init];
    
    self.columnIdentifier = arg1;  // TODO add release link
    self.rowIndex = arg2;
    self.node = (TFENode *)arg3;
    self.nodeStatus = FINodeStatusUnknowned;
    
    [[FIIconOverlay instance] addStatusOpperationToQueue:self];
    
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
    return;
}

- (void) main
{
    if (self != nil)
    {
        //NSURL* url = [self getPath];
        
        // Get status by path
        sleep(1);
        // Set status
        [self setNodeStatus:FINodeStatusDisconected];
        
        [self performSelectorOnMainThread:@selector(refreshCell:) withObject:nil waitUntilDone:NO];
    }
    return;
}

@end