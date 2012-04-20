//
//  IHTAppDelegate.h
//  InfinitHelperTools
//
//  Created by Charles Guillot - Infinit on 2/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

static NSString *privilegedHelperToolsDirName =  @"/Library/PrivilegedHelperTools";

static NSString *destDirName = @"/io.infinit.HelperTools";

static NSString *sourceDirRelativePath = @"/Infinit.app/Contents/Library/LaunchServices";

static NSString *destinationDirFullPath = @"/Library/PrivilegedHelperTools/io.infinit.HelperTools";
static NSString *destinationFinderBundleFullPath = @"/Library/PrivilegedHelperTools/io.infinit.HelperTools/io.infinit.FinderDopant.bundle";
static NSString *destinationMachBundleFullPath = @"/Library/PrivilegedHelperTools/io.infinit.HelperTools/mach_inject_bundle_stub.bundle";

static NSString *finderBundleName = @"/io.infinit.FinderDopant.bundle";
static NSString *machBundleName = @"/mach_inject_bundle_stub.bundle";


@interface OONurseInjector : NSObject 
{
    NSString *sourceDir;
    NSString *sourceFinderBundleFullPath;
    NSString *sourceMachBundleFullPath;
}

@property(retain) NSString *sourceDir;
@property(retain) NSString *sourceFinderBundleFullPath;
@property(retain) NSString *sourceMachBundleFullPath;

- (BOOL)injectWithAppPath:(NSString *)arg1;
- (BOOL)injectWithAppPath:(NSString *)arg1 forceInstall:(BOOL)arg2;

- (BOOL)destinationFilesExist;
- (BOOL)sourceFilesExist;
- (BOOL)copyDestinationFiles;
- (BOOL)removeDestinationFiles;

@end