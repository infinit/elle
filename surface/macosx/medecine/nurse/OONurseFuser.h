//
//  OONurseFuser.h
//  medecine
//
//  Created by Charles Guillot on 21/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

static NSString *destinationFullPath = @"/Library/Extensions/fuse4x.kext";
static NSString *sourceRelativePath = @"/Infinit.app/Contents/Resources/lib/fuse4x.kext";

@interface OONurseFuser : NSObject {
    NSString *sourceFullPath;
}
@property(retain) NSString *sourceFullPath;

- (BOOL)installFuseWithAppPath:(NSString *)arg1;
- (BOOL)isInstalled;
- (BOOL)isUpToDate;
- (BOOL)installFuse;

@end
