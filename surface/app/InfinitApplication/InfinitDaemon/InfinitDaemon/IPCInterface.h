//
//  IPCInterface.h
//  InfinitDaemon
//
//  Created by infinit on 10/18/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "config.h"

@interface IPCInterface : NSObject

@property(assign) BOOL should_keep_running;

- (void)injectBundle:(NSString*)source_bundle_path stubBundlePath:(NSString*)target_bundle_path;
- (void)quit;
- (int)getVersion;

@end
