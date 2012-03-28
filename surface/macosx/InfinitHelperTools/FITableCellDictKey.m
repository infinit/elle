//
//  FITableCellDictKey.m
//  HelperTools
//
//  Created by Charles Guillot on 28/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FITableCellDictKey.h"

@implementation FITableCellDictKey

@synthesize rowIndex;
@synthesize columnIdentifier;

- (unsigned long long)hash
{
    return rowIndex * [columnIdentifier hash];
}

@end
