//
//  InfinitDevices.h
//  Infinit
//
//  Created by Simon Peccaud on 8/22/12.
//  Copyright (c) 2012 infinit.io. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "LoginViewController.h"

@interface InfinitDevices : NSObject

@property (strong) NSMutableDictionary *devices;

+ (InfinitDevices*) singleton;
- (void) fetchDevices;
- (void) fetchDevicesInfo;

@end

@interface InfinitDevice : NSObject

@property (strong) NSString*    external_ip;
@property NSInteger             external_port;
@property (strong) NSString*    local_ip;
@property NSInteger             local_port;

- (id) initWithExternalIp:(NSString*) ext_ip port:(int) ext_port LocalIp:(NSString*) loc_ip port:(int) loc_port;

@end