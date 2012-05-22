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

- (BOOL)installFuseWithAppPath:(NSString *)arg1{
    self.sourceFullPath = arg1;
    if ([self isUpToDate]){
        return YES;
    }
    return [self installFuse];
}

- (BOOL)isInstalled{
    return [[NSFileManager defaultManager] fileExistsAtPath:destinationFullPath];
}

- (BOOL)isUpToDate{
    if ([self isInstalled]) {
        UInt32  sourceBundleVersion;
        UInt32  destBundleVersion;
        
        CFBundleRef sourceBundle = CFBundleCreate(NULL, (CFURLRef)[NSURL fileURLWithPath:self.sourceFullPath]);
        CFBundleRef destBundle = CFBundleCreate(NULL, (CFURLRef)[NSURL fileURLWithPath:destinationFullPath]);
        
        sourceBundleVersion = CFBundleGetVersionNumber(sourceBundle);
        destBundleVersion = CFBundleGetVersionNumber(destBundle);
        return destBundleVersion >= sourceBundleVersion;
    }
    else
        return NO;
}

- (BOOL)installFuse{
    return [[NSFileManager defaultManager] copyItemAtPath:self.sourceFullPath toPath:destinationFullPath error:nil];
}

@end
