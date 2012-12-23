//
//  OONurseFuser.h
//  medecine
//
//  Created by Charles Guillot on 21/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

static NSString *sourceRelativePath = @"/Contents/Resources/fuse4x.bundle";

@interface OONurseFuser : NSObject {
    NSString *sourceFullPath;
    NSError *error; 
}
@property(retain) NSString *sourceFullPath;
@property(retain) NSError *error;

- (BOOL)installFuseWithAppPath:(NSString *)arg1;
- (BOOL)installFuse;

@end
