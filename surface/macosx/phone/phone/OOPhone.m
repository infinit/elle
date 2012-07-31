//
//  phone.m
//  phone
//
//  Created by Charles Guillot on 22/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOPhone.h"
#import "OOManifestParser.h"
#import "OORestResquest.h"

#define countof(X) ( (size_t) ( sizeof(X)/sizeof*(X) ) )

NSString *OOUpdateProgessChangedNotification = @"OOUpdateProgessChangedNotification";

@implementation OOPhone

@synthesize _gap_State;

+(OOPhone *)getInstance{
    
    static OOPhone *singleton;
    @synchronized(self){
        if (!singleton){
            // Le singleton n'a pas encore été instancié
            singleton = [[OOPhone alloc] init];
        }
        return singleton;
    }
    
}

- (id)init {
    if (self = [super init]) {
        self._gap_State = gap_new();
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
       performSelector:(SEL)arg3 
             forObject:(id)arg4 {
    [self addOperationWithBlock:^(void) {
        gap_Status response =  gap_login(self._gap_State,
                                         [arg1 cStringUsingEncoding:NSASCIIStringEncoding],
                                         [arg2 cStringUsingEncoding:NSASCIIStringEncoding]);
        [arg4 performSelectorOnMainThread:arg3 withObject:[NSNumber numberWithInt:response] waitUntilDone:NO];
    }];
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
                                           [arg1 cStringUsingEncoding:NSASCIIStringEncoding],
                                           [arg2 cStringUsingEncoding:NSASCIIStringEncoding],
                                           [arg3 cStringUsingEncoding:NSASCIIStringEncoding],
                                           [arg4 cStringUsingEncoding:NSASCIIStringEncoding],
                                           [arg5 cStringUsingEncoding:NSASCIIStringEncoding]);
        [arg7 performSelectorOnMainThread:arg6 withObject:[NSNumber numberWithInt:response] waitUntilDone:NO];
    }];
}

- (void)createNetworkWithName:(NSString *)arg1 {
    gap_create_network(self._gap_State, [arg1 cStringUsingEncoding:NSASCIIStringEncoding]);
    return;
}

- (void)addUser:(NSString *)arg1 toNetwork:(NSString *)arg2 {
    gap_network_add_user(self._gap_State,
                         [arg2 cStringUsingEncoding:NSASCIIStringEncoding],
                         [arg1 cStringUsingEncoding:NSASCIIStringEncoding]);
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
    char** usersIds = gap_network_users(self._gap_State, [arg1 cStringUsingEncoding:NSASCIIStringEncoding]);
    
    if (usersIds == NULL) return returnArray;
    
    while (*usersIds) {
        char* p = *usersIds;
        NSString *myString = [[NSString alloc] initWithUTF8String:p];
        [returnArray addObject:myString];
        usersIds++;
    }
    return returnArray;
}

- (NSString*)getNetworkNameWithId:(NSString*)arg1 {
    char const* name = gap_network_name(self._gap_State, [arg1 cStringUsingEncoding:NSASCIIStringEncoding]);
    return [[NSString alloc] initWithUTF8String:name];
}

- (void)update {
    OOManifestParser *parser = [[OOManifestParser alloc] init];
    [parser startParse:@"http://download.infinit.im/macosx64/manifest.xml"];
}

//
//  GET USER INFO
//
- (NSString*)getUserFullNameById:(NSString*)arg1 {
    char const* fullName = gap_user_fullname(self._gap_State, [arg1 cStringUsingEncoding:NSASCIIStringEncoding]);
    return [[NSString alloc] initWithUTF8String:fullName];
}


- (NSArray*)searchUsersWithString:(NSString*)arg1 {
    NSMutableArray* returnArray = [[NSMutableArray alloc] init];
    char** usersIds =gap_search_users(self._gap_State, [arg1 cStringUsingEncoding:NSASCIIStringEncoding]);
    
    if (usersIds == NULL) return returnArray;
    
    while (*usersIds) {
        char* p = *usersIds;
        NSString *myString = [[NSString alloc] initWithUTF8String:p];
        [returnArray addObject:myString];
        usersIds++;
    }
    return returnArray;
}
@end
