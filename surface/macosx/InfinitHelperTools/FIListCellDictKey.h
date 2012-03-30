//
//  FIListCellDictKey.h
//  HelperTools
//
//  Created by Charles Guillot on 30/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "FICellDictKey-Protocol.h"

@interface FIListCellDictKey : NSObject <FICellDictKey_Protocol>
{
    id node;
}

@property(nonatomic, retain) id node;

@end
