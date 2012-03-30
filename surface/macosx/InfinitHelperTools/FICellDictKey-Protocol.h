//
//  FICellDictKey-Protocol.h
//  HelperTools
//
//  Created by Charles Guillot on 30/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol FICellDictKey_Protocol <NSObject>

@required
- (void) refreshCell;
- (NSURL *) getPath;

@end
