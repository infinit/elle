//
//  InfinitNetworks.h
//  Infinit
//
//  Created by Simon Peccaud on 8/22/12.
//  Copyright (c) 2012 infinit.io. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "LoginViewController.h"

@interface InfinitNetworks : NSObject

@property (strong) NSMutableDictionary *networks;

+ (InfinitNetworks*) singleton;
+ (void) destroySingleton;
- (void) fetchNetworks;
- (void) fetchNetworksInfo;

@end

@interface InfinitNetwork : NSObject

@property (strong) NSString*            name;
@property (strong) NSMutableArray*      devices;
@property (strong) NSMutableArray*      users;

- (id) initWithName:(NSString*) network_name devices:(NSMutableArray*) network_devices users:(NSMutableArray*) network_users;

@end