//
//  ZombieDictKey.m
//  medecine
//
//  Created by Charles Guillot on 02/04/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ZombieDictKey.h"

@implementation ZombieDictKey

//static NSMutableDictionary *items = nil;

@synthesize linkedDictKeys;

/*+ (id) ZombieWithCell:(id)arg1
{
    @synchronized(items)
    {
        if (items == nil) {
            items = [[NSMutableDictionary alloc] init];
        }
        
        ZombieDictKey *item = nil;
        item = [items objectForKey:arg1];
        
        if (item == nil)
        {
            item = [[ZombieDictKey alloc] initWithCell:arg1];
            [items setObject:item forKey:arg1];
        }
        
        return item;
    }
}*/

- (id) init
{
    self = [super init];
    
    if (!linkedDictKeys) {
        linkedDictKeys = [[NSMutableArray alloc] init];
    }
    
    return self;
}

- (void) addDictKey:(id)arg1
{
    [self.linkedDictKeys addObject:arg1];
}

-(void)dealloc
{
    for (id<FICellDictKey_Protocol> dictKeys in self.linkedDictKeys) {
        [dictKeys cleanUp];
    }
    [super dealloc];
}

@end
