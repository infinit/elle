//
//  FIListCellDictKey.m
//  HelperTools
//
//  Created by Charles Guillot on 30/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FIListCellDictKey.h"

@implementation FIListCellDictKey

@synthesize cell;
@synthesize nodeStatus;

static NSMutableSet *items = nil;

+ (id)listCellDictKeyWithCell:(id)arg1
{
    @synchronized(arg1)
    {
        if (items == nil) {
            items = [[NSMutableSet alloc] init];
        }
        id item = [[FIListCellDictKey alloc] initWithCell:arg1];
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

- (id) initWithCell:(id)arg1
{
    self = [super init];
    
    self.cell = arg1;
    self.nodeStatus = FINodeStatusUnknowned;
    
    return self;
}
-(void)dealloc
{
    if ([items containsObject:self]){
        [items removeObject:self];
    }
    [cell release];
    [super dealloc];
}
- (unsigned long long)hash
{
    return [self.cell hash];
}
- (BOOL)isEqual:(id)arg1
{
    if (arg1 == self)
        return YES;
    if (!arg1 || ![arg1 isKindOfClass:[self class]])
        return NO;
    return [self.cell isEqual:((FIListCellDictKey *)arg1).cell];
}

- (NSURL *) getPath
{
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
    // TODO refresh cell
    return;
}



@end
