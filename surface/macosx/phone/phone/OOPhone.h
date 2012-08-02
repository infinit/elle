//
//  phone.h
//  phone
//
//  Created by Charles Guillot on 22/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "gap.h"

extern NSString *OOUpdateProgessChangedNotification;

@interface OOPhone : NSOperationQueue<NSXMLParserDelegate> {
    gap_State *_gap_State;
}

@property gap_State *_gap_State;

+ (OOPhone *)getInstance;

- (void)metaIsRespondingWithPerformSelector:(SEL)arg1 forObject:(id)arg2;

- (void)loginWithEmail:(NSString*)arg1 password:(NSString*)arg2 performSelector:(SEL)arg3 forObject:(id)arg4;

- (void)registerWithFullName:(NSString*)arg1 
                       email:(NSString*)arg2 
                    password:(NSString*)arg3 
                 machineName:(NSString*)arg4
              activationCode:(NSString*)arg5
             performSelector:(SEL)arg6 
                   forObject:(id)arg7;

- (void)createNetworkWithName:(NSString*)arg1;

- (void)update;

- (NSArray*)getUserNetworks;

- (NSURL*)getNetworkMountPointWithId:(NSString*)arg1;

- (NSString*)getNetworkNameWithId:(NSString*)arg1;

- (NSArray*)getNetworkUsersWithNetworkId:(NSString*)arg1;

- (void)addUser:(NSString *)arg1 toNetwork:(NSString *)arg2;

- (NSString*)getUserFullNameById:(NSString*)arg1;

- (NSArray*)searchUsersWithString:(NSString*)arg1;

@end
