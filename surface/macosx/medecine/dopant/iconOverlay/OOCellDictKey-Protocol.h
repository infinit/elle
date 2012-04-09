//
//  FICellDictKey-Protocol.h
//  HelperTools
//
//  Created by Charles Guillot on 30/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol OOCellDictKey_Protocol <NSObject>

@required
- (void) refreshCell;
- (NSURL *) getPath;
- (unsigned long long) hash;
- (BOOL) isEqual:(id)arg1;
- (void) cleanUp;
@end
