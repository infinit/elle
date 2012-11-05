//
//  InfinitNetworks.m
//  Infinit
//
//  Created by Simon Peccaud on 8/22/12.
//  Copyright (c) 2012 infinit.io. All rights reserved.
//

#import "InfinitNetworks.h"

@implementation InfinitNetworks

static InfinitNetworks*  networksManager = nil;

- (id) init {
    self.networks = [[NSMutableDictionary alloc] init];
    [self fetchNetworks];
    [self fetchNetworksInfo];

    return self;
}

- (void) fetchNetworks {
    NSURL *url = [NSURL URLWithString:@"http://infinit.im:12345/networks"];

    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];
    NSError *error = NULL;
    NSURLResponse *response = nil;

    [request setURL:url];
    [request setHTTPMethod:@"GET"];
    [request setValue:token forHTTPHeaderField:@"AUTHORIZATION"];
    [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];

    [request setCachePolicy:NSURLRequestReloadIgnoringCacheData];

    NSData *result = [NSURLConnection sendSynchronousRequest:request
                                           returningResponse:&response error:&error];

    NSString *resultString = [[NSString alloc] initWithData:result encoding:NSUTF8StringEncoding];
  //  NSLog(@"\n\t=> %@ \n\t=> %@ \n\t=> %@", resultString, error.userInfo, response.description);

    NSArray *networkIds = [[resultString objectFromJSONString] objectForKey:@"networks"];
    NSEnumerator    *networkIdEnum = [networkIds objectEnumerator];
    NSString *networkId = nil;

    while (networkId = [networkIdEnum nextObject]) {
        [self.networks setValue:[[NSMutableDictionary alloc] init] forKey:networkId];
    }
   // NSLog(@"Networks :=%@=", self.networks);
}

- (void) fetchNetworksInfo {
    NSEnumerator* idsEnum = [[self.networks allKeys] objectEnumerator];

    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];
    NSError *error = NULL;
    NSURLResponse *response = nil;
    NSString* networkId;
    
    [request setHTTPMethod:@"GET"];
    [request setValue:token forHTTPHeaderField:@"AUTHORIZATION"];
    [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
    [request setCachePolicy:NSURLRequestReloadIgnoringCacheData];
    
    while (networkId = [idsEnum nextObject]) {
        NSURL *url = [NSURL URLWithString:[@"http://infinit.im:12345/network/" stringByAppendingFormat:@"%@/view", networkId]];
        [request setURL:url];
        NSData *result = [NSURLConnection sendSynchronousRequest:request
                                               returningResponse:&response error:&error];
        NSString *resultString = [[NSString alloc] initWithData:result encoding:NSUTF8StringEncoding];
     //   NSLog(@"\n\t=> %@ \n\t=> %@ \n\t=> %@", resultString, error.userInfo, response.description);
        NSMutableDictionary* networkDico = [[NSMutableDictionary alloc] initWithDictionary:[resultString objectFromJSONString] copyItems:TRUE];
        
        if (networkDico && (CFBooleanGetValue((__bridge CFBooleanRef)([networkDico valueForKey:@"success"])))) {
            InfinitNetwork*  ifNetwork = [[InfinitNetwork alloc] initWithName:[networkDico objectForKey:@"name"]
                                                                            devices:[networkDico objectForKey:@"devices"]
                                                                            users:[networkDico objectForKey:@"users"]];
            [self.networks setObject:ifNetwork forKey:networkId];
        }
    }
}

// Singleton methods
+ (InfinitNetworks*) singleton {
    if (networksManager == nil) {
        networksManager = [[super allocWithZone:NULL] init];
    }
    return networksManager;
}
+ (void) destroySingleton {
    [networksManager.networks removeAllObjects];
    networksManager = nil;
}

+ (id)allocWithZone:(NSZone *)zone {
    return [self singleton];
}

- (id)copyWithZone:(NSZone *)zone {
    return self;
}

@end

@implementation InfinitNetwork

- (id) initWithName:(NSString*) network_name devices:(NSMutableArray*) network_devices users:(NSMutableArray*) network_users {
    self.name = network_name;
    self.devices = network_devices;
    self.users = network_users;
    
    return self;
}

@end