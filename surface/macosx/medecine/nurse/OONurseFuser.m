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

- (BOOL)installFuseWithAppPath:(NSString *)arg1{
    self.sourceFullPath = [arg1 stringByAppendingString:sourceRelativePath];
    if ([self isUpToDate]){
        NSLog(@"Fuse kext version is up to date");
        return YES;
    }
    if ([self installFuse])
    {
        NSLog(@"Fuse kext is installed");
        return YES;
    }
    else {
        NSLog(@"Fuse kext failled to install %@ at %@. Error:%@",self.sourceFullPath, destinationFullPath,  error);
        return NO;
    }
}

- (BOOL)isInstalled{
    return [[NSFileManager defaultManager] fileExistsAtPath:destinationFullPath];
}

- (BOOL)isUpToDate{
    if ([self isInstalled]) {
        
        NSString *sourceBundleVersion = [[[NSBundle bundleWithPath:self.sourceFullPath] infoDictionary] valueForKey:@"CFBundleVersion"];
        NSString *destBundleVersion = [[[NSBundle bundleWithPath:destinationFullPath] infoDictionary] valueForKey:@"CFBundleVersion"];
        
        return destBundleVersion >= sourceBundleVersion;
    }
    else
        return NO;
}

- (BOOL)installFuse{
    return [[NSFileManager defaultManager] copyItemAtPath:self.sourceFullPath toPath:destinationFullPath error:&error];
}

@end
