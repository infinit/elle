//
//  OOCellDictKey-Protocol.h
//  Dopant
//
//  Created by Charles Guillot on 30/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

/**
 This protocol template the dictkey classes
 */
@protocol OOCellDictKey_Protocol <NSObject>

@required
- (void) refreshCell;
- (NSURL *) getPath;
- (unsigned long long) hash;
- (BOOL) isEqual:(id)arg1;
- (void) cleanUp;

@end
