//
//  IHTAppDelegate.h
//  InfinitHelperTools
//
//  Created by Charles Guillot - Infinit on 2/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

static NSString *destinationDirectory = @"/Library/PrivilegedHelperTools/io.infinit.HelperTools";

static NSString *finderBundleDestination = @"/Library/PrivilegedHelperTools/io.infinit.HelperTools/io.infinit.FinderInjectee.bundle";
static NSString *finderBundleSource = @"/Applications/Infinit.app/Contents/Library/LaunchServices/io.infinit.FinderInjectee.bundle";


static NSString *machBundleDestination = @"/Library/PrivilegedHelperTools/io.infinit.HelperTools/mach_inject_bundle_stub.bundle";
static NSString *machBundleSource = @"/Applications/Infinit.app/Contents/Library/LaunchServices/mach_inject_bundle_stub.bundle";

@interface IHTInjector : NSObject 

- (BOOL)inject;
- (BOOL)inject:(BOOL)forceInstallBundle;
- (BOOL)createDirectory;
- (BOOL)installFinderBundle;
- (BOOL)installMachBundle;

@end