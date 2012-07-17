//
//  OORestResquest.h
//  phone
//
//  Created by Charles Guillot on 27/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface OORestResquest : NSObject {
    NSString* token;
    
}

@property(retain) NSString* token;

+ (OORestResquest *)getInstance;

- (NSString*)getTokenWithEmail:(NSString*)arg1 
                      password:(NSString*)arg2;

- (NSData* )sendSynchronousRequestWithRelativeUrl:(NSString*)arg1
                                         jsonDict:(NSDictionary*)arg2
                                       HTTPMethod:(NSString*)arg3
                                            error:(NSError*)arg4;
- (BOOL)registerWithEmail:(NSString*)arg1
                 fullName:(NSString*)arg2 
                 password:(NSString*)arg3 
               adminToken:(NSString*)arg4;

- (NSString*) createNetworkWithName:(NSString*)arg1 withUsers:(NSArray*)arg2;

- (BOOL)registerDeviceWithName:(NSString*)arg1;

- (BOOL)logout;

- (BOOL)isLogged;

@end
