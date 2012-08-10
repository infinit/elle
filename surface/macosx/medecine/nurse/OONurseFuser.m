//
//  OONurseFuser.m
//  medecine
//
//  Created by Charles Guillot on 21/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OONurseFuser.h"

@implementation OONurseFuser

@synthesize sourceFullPath;
@synthesize error;

- (BOOL)installFuseWithAppPath:(NSString *)arg1 {
    self.sourceFullPath = [arg1 stringByAppendingString:sourceRelativePath];
    return [self installFuse];
}

- (BOOL)installFuse {
    // copy to /usr/local/lib
    BOOL libInstalled = [self copyForEachElementsIn:[self.sourceFullPath stringByAppendingString:@"/usr/local/lib"] to:@"/usr/local/lib"];
    // copy to /usr/local/include
    BOOL includeInstalled =[self copyForEachElementsIn:[self.sourceFullPath stringByAppendingString:@"/usr/local/include"] to:@"/usr/local/include"];
    // copy to /Library/Extensions
    BOOL kextInstalled =[self copyForEachElementsIn:[self.sourceFullPath stringByAppendingString:@"/Library/Extensions"] to:@"/Library/Extensions"];
    
    return libInstalled && includeInstalled && kextInstalled;
}

- (BOOL)copyForEachElementsIn:(NSString*)arg1 to:(NSString*)arg2 {
    NSFileManager *fm = [NSFileManager defaultManager];
    NSArray *dirContents = [fm contentsOfDirectoryAtPath:arg1 error:nil];
    BOOL result = YES;
    for(NSString * item in dirContents) {
        [fm removeItemAtPath:[arg2 stringByAppendingPathComponent:item] error:&error];
        result &= [fm copyItemAtPath:[arg1 stringByAppendingPathComponent:item]
                              toPath:[arg2 stringByAppendingPathComponent:item]
                               error:&error];
    }
    return result;
}

@end
