//
//  FIGetStatusOperationQueue.h
//  HelperTools
//
//  Created by Charles Guillot on 29/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface FIGetStatusOperationQueue : NSOperation
{
    id dictKey;
}
@property(readonly) id dictKey;


@end
