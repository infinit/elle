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
    NSMutableArray *linkedDictKeys;
    id      *parentCell;
}

@property(retain) NSMutableArray *linkedDictKeys;

- (void) addDictKey:(id)arg1;
- (id) init;

@end
