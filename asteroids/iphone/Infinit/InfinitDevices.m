//
//  InfinitDevices.m
//  Infinit
//
//  Created by Simon Peccaud on 8/22/12.
//  Copyright (c) 2012 infinit.io. All rights reserved.
//

#import "InfinitDevices.h"

@implementation InfinitDevices

static InfinitDevices*  devicesManager = nil;

@synthesize devices;

- (id) init {
    self.devices = [[NSMutableDictionary alloc] init];
    [self fetchDevices];
    [self fetchDevicesInfo];

    return self;
}

- (void) fetchDevices {
    NSURL *url = [NSURL URLWithString:@"http://infinit.im:12345/devices"];

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
   // NSLog(@"\n\t=> %@ \n\t=> %@ \n\t=> %@", resultString, error.userInfo, response.description);

    NSArray *deviceIds = [[resultString objectFromJSONString] objectForKey:@"devices"];
    NSEnumerator    *deviceIdEnum = [deviceIds objectEnumerator];
    NSString *deviceId = nil;

    while (deviceId = [deviceIdEnum nextObject]) {
        [devices setValue:[[NSMutableDictionary alloc] init] forKey:deviceId];
    }
    // NSLog(@"Devices :=%@=", devices);
}

- (void) fetchDevicesInfo {
    NSEnumerator* idsEnum = [[devices allKeys] objectEnumerator];

    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];
    NSError *error = NULL;
    NSURLResponse *response = nil;
    NSString* deviceId;

    [request setHTTPMethod:@"GET"];
    [request setValue:token forHTTPHeaderField:@"AUTHORIZATION"];
    [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
    [request setCachePolicy:NSURLRequestReloadIgnoringCacheData];

    while (deviceId = [idsEnum nextObject]) {
        NSURL *url = [NSURL URLWithString:[@"http://infinit.im:12345/device/" stringByAppendingFormat:@"%@/view", deviceId]];
        [request setURL:url];
        NSData *result = [NSURLConnection sendSynchronousRequest:request
                                           returningResponse:&response error:&error];
        NSString *resultString = [[NSString alloc] initWithData:result encoding:NSUTF8StringEncoding];
       // NSLog(@"\n\t=> %@ \n\t=> %@ \n\t=> %@", resultString, error.userInfo, response.description);
        NSMutableDictionary* networkDico = [[NSMutableDictionary alloc] initWithDictionary:[resultString objectFromJSONString] copyItems:TRUE];

        if (networkDico && (CFBooleanGetValue((__bridge CFBooleanRef)([networkDico valueForKey:@"success"])))) {
            NSDictionary* ext_info = [networkDico objectForKey:@"extern_address"];
            NSDictionary* loc_info = [networkDico objectForKey:@"local_address"];
            InfinitDevice*  ifDevice = [[InfinitDevice alloc] initWithExternalIp:[ext_info objectForKey:@"ip"]
                                                                            port:(int) [ext_info objectForKey:@"port"]
                                                                         LocalIp:[loc_info objectForKey:@"ip"]
                                                                            port:(int) [loc_info objectForKey:@"port"]];
            [devices setObject:ifDevice forKey:deviceId];
        }
    }
}

// Singleton methods
+ (InfinitDevices*) singleton {
    if (devicesManager == nil) {
        devicesManager = [[super allocWithZone:NULL] init];
    }
    return devicesManager;
}

+ (id)allocWithZone:(NSZone *)zone {
    return [self singleton];
}

- (id)copyWithZone:(NSZone *)zone {
    return self;
}

@end

@implementation InfinitDevice
@synthesize external_ip;
@synthesize external_port;
@synthesize local_ip;
@synthesize local_port;

- (id) initWithExternalIp:(NSString*) ext_ip port:(int) ext_port LocalIp:(NSString*) loc_ip port:(int) loc_port {
    self.external_ip = ext_ip;
    self.external_port = ext_port;
    self.external_ip = ext_ip;
    self.external_port = ext_port;
    
    return self;
}

@end