//
//  ZombieDictKey.h
//  medecine
//
//  Created by Charles Guillot on 02/04/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "FICellDictKey-Protocol.h"

@interface ZombieDictKey : NSObject
{
    NSMutableArray  *linkedDictKeys;
    id              parent;
}

@property(retain) NSMutableArray *linkedDictKeys;
@property(assign) id parent;

- (void) addDictKey:(id)arg1;
- (id) init;

@end
