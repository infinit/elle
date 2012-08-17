//
//  phone.m
//  phone
//
//  Created by Charles Guillot on 22/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOPhone.h"
#import "OOManifestParser.h"

#define countof(X) ( (size_t) ( sizeof(X)/sizeof*(X) ) )

NSString *OOUpdateProgessChangedNotification = @"OOUpdateProgessChangedNotification";

@implementation OOPhone

@synthesize _gap_State;

+(OOPhone *)getInstance{
    
    static OOPhone *singleton;
    @synchronized(self){
        if (!singleton){
            // Singleton has not been instancied
            singleton = [[OOPhone alloc] init];
        }
        return singleton;
    }
    
}

- (id)init {
    if (self = [super init]) {
        self._gap_State = gap_new();
        gap_enable_debug(self._gap_State);
    }
    return self;
}

- (void)dealloc {
    [self addOperationWithBlock:^(void) {
        gap_free(self._gap_State);
    }];
}

- (void)metaIsRespondingWithPerformSelector:(SEL)arg1 forObject:(id)arg2 {
    [self addOperationWithBlock:^(void) {
        gap_Status response = gap_meta_status(self._gap_State);
        [arg2 performSelectorOnMainThread:arg1 withObject:[NSNumber numberWithInt:response] waitUntilDone:NO];
    }];
}

- (void)loginWithEmail:(NSString*)arg1 
              password:(NSString*)arg2
           machineName:(NSString*)arg3
       performSelector:(SEL)arg4
             forObject:(id)arg5 {
    [self addOperationWithBlock:^(void) {
        gap_logout(self._gap_State);
        gap_Status response =  gap_login(self._gap_State,
                                         [arg1 cStringUsingEncoding:NSUTF8StringEncoding],
                                         [arg2 cStringUsingEncoding:NSUTF8StringEncoding]);
        [self registerDeviceWithDeviceName:arg3];
        [arg5 performSelectorOnMainThread:arg4 withObject:[NSNumber numberWithInt:response] waitUntilDone:NO];
    }];
}

- (NSString *)getHashPasswordWithEmail:(NSString*)arg1 andClearPassword:(NSString*)arg2 {
    char* hashPassword = gap_hash_password(self._gap_State,
                            [arg1 cStringUsingEncoding:NSUTF8StringEncoding],
                            [arg2 cStringUsingEncoding:NSUTF8StringEncoding]);
    NSString* returnValue = [[NSString alloc] initWithUTF8String:hashPassword];
    gap_hash_free(hashPassword);
    return returnValue;
}

- (void)registerWithFullName:(NSString*)arg1
                       email:(NSString*)arg2 
                    password:(NSString*)arg3 
                 machineName:(NSString*)arg4 
              activationCode:(NSString*)arg5 
             performSelector:(SEL)arg6 
                   forObject:(id)arg7 {
    [self addOperationWithBlock:^(void) {
        gap_logout(self._gap_State);
        gap_Status response = gap_register(self._gap_State,
                                           [arg1 cStringUsingEncoding:NSUTF8StringEncoding],
                                           [arg2 cStringUsingEncoding:NSUTF8StringEncoding],
                                           [arg3 cStringUsingEncoding:NSUTF8StringEncoding],
                                           [arg4 cStringUsingEncoding:NSUTF8StringEncoding],
                                           [arg5 cStringUsingEncoding:NSUTF8StringEncoding]);
        [self registerDeviceWithDeviceName:arg4];
        [arg7 performSelectorOnMainThread:arg6 withObject:[NSNumber numberWithInt:response] waitUntilDone:NO];
    }];
}

- (void)registerDeviceWithDeviceName:(NSString*)arg1 {
    if (gap_device_status(self._gap_State) != gap_ok) {
        gap_set_device_name(self._gap_State, [arg1 cStringUsingEncoding:NSUTF8StringEncoding]);
    }
}

- (void)createNetworkWithName:(NSString *)arg1
              performSelector:(SEL)arg2
                    forObject:(id)arg3 {
    [self addOperationWithBlock:^(void) {
        gap_create_network(self._gap_State, [arg1 cStringUsingEncoding:NSUTF8StringEncoding]);
        [arg3 performSelectorOnMainThread:arg2 withObject:nil waitUntilDone:NO];
    }];
    return;
}

- (void)addUser:(NSString *)arg1 toNetwork:(NSString *)arg2 {
    gap_network_add_user(self._gap_State,
                         [arg2 cStringUsingEncoding:NSUTF8StringEncoding],
                         [arg1 cStringUsingEncoding:NSUTF8StringEncoding]);
    return;
}

- (NSArray*)getUserNetworks {
    NSMutableArray* returnArray = [[NSMutableArray alloc] init];
    char** networkIds = gap_networks(self._gap_State);
    
    if (networkIds == NULL) return returnArray;
    
    while (*networkIds) {
        char* p = *networkIds;
        NSString *myString = [[NSString alloc] initWithUTF8String:p];
        [returnArray addObject:myString];
        networkIds++;
    }
    return returnArray;
}

- (NSArray*)getNetworkUsersWithNetworkId:(NSString*)arg1 {
    NSMutableArray* returnArray = [[NSMutableArray alloc] init];
    char** usersIds = gap_network_users(self._gap_State, [arg1 cStringUsingEncoding:NSUTF8StringEncoding]);
    
    if (usersIds == NULL) return returnArray;
    
    while (*usersIds) {
        char* p = *usersIds;
        NSString *myString = [[NSString alloc] initWithUTF8String:p];
        [returnArray addObject:myString];
        usersIds++;
    }
    return returnArray;
}

//
//  GET USER INFO
//
- (NSString*)getNetworkNameWithId:(NSString*)arg1 {
    char const* name = gap_network_name(self._gap_State, [arg1 cStringUsingEncoding:NSUTF8StringEncoding]);
    return [[NSString alloc] initWithUTF8String:name];
}

- (NSURL*)getNetworkMountPointWithId:(NSString*)arg1 {
    char const* path = gap_network_mount_point(self._gap_State, [arg1 cStringUsingEncoding:NSUTF8StringEncoding]);
    if (path == nil)return nil;
    return [NSURL fileURLWithPath:[[NSString alloc] initWithUTF8String:path]];
}

- (void)update {
    OOManifestParser *parser = [[OOManifestParser alloc] init];
    [parser startParse:@"http://download.infinit.im/macosx64/manifest.xml"];
}

//
//  GET USER INFO
//
- (NSString*)getUserFullNameById:(NSString*)arg1 {
    char const* fullName = gap_user_fullname(self._gap_State, [arg1 cStringUsingEncoding:NSUTF8StringEncoding]);
    return [[NSString alloc] initWithUTF8String:fullName];
}


- (NSArray*)searchUsersWithString:(NSString*)arg1 {
    NSMutableArray* returnArray = [[NSMutableArray alloc] init];
    char** usersIds = gap_search_users(self._gap_State, [arg1 cStringUsingEncoding:NSUTF8StringEncoding]);
    
    if (usersIds == NULL) return returnArray;
    
    while (*usersIds) {
        char* p = *usersIds;
        NSString *myString = [[NSString alloc] initWithUTF8String:p];
        [returnArray addObject:myString];
        usersIds++;
    }
    return returnArray;
}

//
//  Launch watchdog
//
- (void)launchWatchdog {
    gap_launch_watchdog(self._gap_State);
}
//
//  Stop watchdog
//
- (void)stopWatchdog {
    gap_stop_watchdog(self._gap_State);
}
@end
