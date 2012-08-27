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
    NSFileManager *fm = [NSFileManager defaultManager];
    BOOL isDir;
    if(![fm fileExistsAtPath:@"usr/local/lib" isDirectory:&isDir])
        if(![fm createDirectoryAtPath:@"usr/local/lib" withIntermediateDirectories:YES attributes:nil error:nil])
            NSLog(@"Error: Create folder failed");
    if(![fm fileExistsAtPath:@"usr/local/include" isDirectory:&isDir])
        if(![fm createDirectoryAtPath:@"usr/local/include" withIntermediateDirectories:YES attributes:nil error:nil])
            NSLog(@"Error: Create folder failed");
    // copy to /usr/local/lib
    BOOL libInstalled = [self copyForEachElementsIn:[self.sourceFullPath stringByAppendingString:@"/usr/local/lib"] to:@"/usr/local/lib" withForceCopy:NO];
    libInstalled &= [self copyForEachElementsIn:[self.sourceFullPath stringByAppendingString:@"/usr/local/lib"] to:@"/usr/local/lib" withForceCopy:NO];
    // copy to /usr/local/include
    BOOL includeInstalled =[self copyForEachElementsIn:[self.sourceFullPath stringByAppendingString:@"/usr/local/include"] to:@"/usr/local/include" withForceCopy:NO];
    // copy to /Library/Extensions
    BOOL kextInstalled =[self copyForEachElementsIn:[self.sourceFullPath stringByAppendingString:@"/Library/Extensions"] to:@"/Library/Extensions" withForceCopy:NO];
    
    NSDictionary *attr=[NSDictionary dictionaryWithObject:[NSNumber numberWithUnsignedLong:06755UL] forKey:NSFilePosixPermissions];
    
    [fm setAttributes:attr ofItemAtPath:@"/Library/Extensions/fuse4x.kext/Support/load_fuse4x" error:&error];
    
    return libInstalled && includeInstalled && kextInstalled;
}

- (BOOL)copyForEachElementsIn:(NSString*)arg1 to:(NSString*)arg2 withForceCopy:(BOOL)arg3{
    NSFileManager *fm = [NSFileManager defaultManager];
    NSArray *dirContents = [fm contentsOfDirectoryAtPath:arg1 error:nil];
    NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
                          [NSNumber numberWithInt:0], NSFileGroupOwnerAccountID,
                          [NSNumber numberWithInt:0], NSFileOwnerAccountID,
                          @"root", NSFileGroupOwnerAccountName,
                          @"root", NSFileOwnerAccountName, nil ];
    
    BOOL result = YES;
    for(NSString * item in dirContents) {
        if (arg3) {
            [fm removeItemAtPath:[arg2 stringByAppendingPathComponent:item] error:&error];
        }
        if (![fm fileExistsAtPath:[arg2 stringByAppendingPathComponent:item]]) {
            result &= [fm copyItemAtPath:[arg1 stringByAppendingPathComponent:item]
                                  toPath:[arg2 stringByAppendingPathComponent:item]
                                   error:&error];
            [self setPermissionAtPath:[arg2 stringByAppendingPathComponent:item] withAttributes:attr];
        }
    }
    return result;
}
- (BOOL)setPermissionAtPath:(NSString*)arg1 withAttributes:(NSDictionary*)arg2 {
    NSDictionary *fileAttr=[NSDictionary dictionaryWithObject:[NSNumber numberWithUnsignedLong:0644] forKey:NSFilePosixPermissions];
    NSDictionary *dirAttr=[NSDictionary dictionaryWithObject:[NSNumber numberWithUnsignedLong:0751] forKey:NSFilePosixPermissions];
    NSFileManager *fm = [NSFileManager defaultManager];
    [fm setAttributes:arg2 ofItemAtPath:arg1 error:&error];
    BOOL isDir;
    
    if([[NSFileManager defaultManager] fileExistsAtPath:arg1
                                            isDirectory:&isDir] && isDir) {
        [fm setAttributes:dirAttr ofItemAtPath:arg1 error:&error];
    } else {
        [fm setAttributes:fileAttr ofItemAtPath:arg1 error:&error];
    }
    NSArray *dirItem = [fm contentsOfDirectoryAtPath:arg1 error:nil];
    for(NSString * item in dirItem) {
        [self setPermissionAtPath:[arg1 stringByAppendingPathComponent:item] withAttributes:arg2];
    }
    NSLog(@"content dir : %@",arg1);
    return YES;
}
@end
