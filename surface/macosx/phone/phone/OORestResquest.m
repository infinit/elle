//
//  OORestResquest.m
//  phone
//
//  Created by Charles Guillot on 27/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OORestResquest.h"

NSString* infinitAPIURL = @"http://infinit.im:12345/";

@implementation OORestResquest

@synthesize token;

+(OORestResquest *)getInstance{
    
    static OORestResquest *singleton;
    @synchronized(self){
        if (!singleton){
            // Le singleton n'a pas encore été instancié
            singleton = [[OORestResquest alloc] init];
        }
        return singleton;
    }
    
}

- (NSData* )sendSynchronousRequestWithRelativeUrl:(NSString*)arg1
                                         jsonDict:(NSDictionary*)arg2
                                       HTTPMethod:(NSString*)arg3
                                            error:(NSError*)arg4 {
    NSData* __jsonData;
    NSString* __jsonString;
    
    if([NSJSONSerialization isValidJSONObject:arg2])
    {
        __jsonData = [NSJSONSerialization dataWithJSONObject:arg2 options:0 error:nil];
        __jsonString = [[NSString alloc]initWithData:__jsonData encoding:NSUTF8StringEncoding];
    }
    
    NSURL *requestUrl = [NSURL URLWithString:[infinitAPIURL stringByAppendingString:arg1]];
    // Be sure to properly escape your url string.
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:requestUrl];
    [request setHTTPMethod:arg3];
    [request setHTTPBody: __jsonData];
    [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
    [request setValue:[NSString stringWithFormat:@"%d", [__jsonData length]] forHTTPHeaderField:@"Content-Length"];  
    
    NSURLResponse *theResponse =[[NSURLResponse alloc]init];
    NSData *data = [NSURLConnection sendSynchronousRequest:request returningResponse:&theResponse error:&arg4];
    return data;
}

- (NSString*)getTokenWithEmail:(NSString*)arg1 password:(NSString*)arg2 {
    NSError *errorReturned = nil;
    NSArray *keys = [NSArray arrayWithObjects:@"email", @"password", nil];
    NSArray *objects = [NSArray arrayWithObjects:arg1, arg2, nil];
    
    NSDictionary *jsonDictionary = [NSDictionary dictionaryWithObjects:objects forKeys:keys];
    NSData* data = [self sendSynchronousRequestWithRelativeUrl:@"login"
                                                      jsonDict:jsonDictionary
                                                    HTTPMethod:@"POST"
                                                         error:errorReturned];
    if (errorReturned) {
        return Nil;
    }
    else
    {
        NSError *jsonParsingError = nil;
        NSArray *jsonArray = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers|NSJSONReadingAllowFragments error:&jsonParsingError];
        self.token = (NSString *)[jsonArray valueForKey:@"token"];
        return self.token;
    }
}

- (BOOL)registerWithEmail:(NSString*)arg1
                 fullName:(NSString*)arg2 
                 password:(NSString*)arg3 
               adminToken:(NSString*)arg4 {
    [self logout];
    NSError *errorReturned = nil;
    NSArray *keys = [NSArray arrayWithObjects:@"email", @"fullname", @"password", @"admin_token", nil];
    NSArray *objects = [NSArray arrayWithObjects:arg1, arg2, arg3, arg4, nil];
    
    NSDictionary *jsonDictionary = [NSDictionary dictionaryWithObjects:objects forKeys:keys];
    
    NSData* data = [self sendSynchronousRequestWithRelativeUrl:@"register"
                                                      jsonDict:jsonDictionary
                                                    HTTPMethod:@"POST"
                                                         error:errorReturned];
    if (errorReturned) {
        return NO;
    }
    else
    {
        NSError *jsonParsingError = nil;
        NSDictionary *jsonArray = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers|NSJSONReadingAllowFragments error:&jsonParsingError];
        if (!jsonArray) { return NO; };
        if (![jsonArray isKindOfClass:[NSDictionary class]]) {
            return NO;
        }
        NSNumber* aaa = [jsonArray objectForKey:@"success"];
        //NSString* err = [jsonArray objectForKey:@"error"];
        
        return [aaa boolValue];
    }
}

- (BOOL)registerDeviceWithName:(NSString*)arg1 {
    [self isLogged];
    NSError *errorReturned = nil;
    NSArray *keys = [NSArray arrayWithObjects:@"name", nil];
    NSArray *objects = [NSArray arrayWithObjects:arg1, nil];
    
    NSDictionary *jsonDictionary = [NSDictionary dictionaryWithObjects:objects forKeys:keys];
    
    NSData* data = [self sendSynchronousRequestWithRelativeUrl:@"device"
                                                      jsonDict:jsonDictionary
                                                    HTTPMethod:@"POST"
                                                         error:errorReturned];
    if (errorReturned) {
        return NO;
    }
    else
    {
        NSError *jsonParsingError = nil;
        NSDictionary *jsonArray = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers|NSJSONReadingAllowFragments error:&jsonParsingError];
        if (!jsonArray) { return NO; };
        if (![jsonArray isKindOfClass:[NSDictionary class]]) {
            return NO;
        }
        NSNumber* aaa = [jsonArray objectForKey:@"success"];
        //NSString* err = [jsonArray objectForKey:@"error"];
        
        return [aaa boolValue];
    }
}


- (BOOL)loggout {
    NSError *errorReturned = nil;
    
    NSData* data = [self sendSynchronousRequestWithRelativeUrl:@"logout"
                                                      jsonDict:Nil
                                                    HTTPMethod:@"GET"
                                                         error:errorReturned];
    if (errorReturned) {
        return NO;
    }
    else
    {
        NSError *jsonParsingError = nil;
        NSDictionary *jsonArray = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers|NSJSONReadingAllowFragments error:&jsonParsingError];
        if (!jsonArray) { return NO; };
        if (![jsonArray isKindOfClass:[NSDictionary class]]) {
            return NO;
        }
        NSNumber* success = [jsonArray objectForKey:@"success"];
        //NSString* err = [jsonArray objectForKey:@"error"];
        
        return [success boolValue];
    }
}


- (BOOL)isLogged {
    NSError *errorReturned = nil;
    
    NSData* data = [self sendSynchronousRequestWithRelativeUrl:@""
                                                      jsonDict:Nil
                                                    HTTPMethod:@"GET"
                                                         error:errorReturned];
    if (errorReturned) {
        return NO;
    }
    else
    {
        NSError *jsonParsingError = nil;
        NSDictionary *jsonArray = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers|NSJSONReadingAllowFragments error:&jsonParsingError];
        if (!jsonArray) { return NO; };
        if (![jsonArray isKindOfClass:[NSDictionary class]]) {
            return NO;
        }
        NSNumber* success = [jsonArray objectForKey:@"logged_in"];
        //NSString* err = [jsonArray objectForKey:@"error"];
        
        return [success boolValue];
    }
}

- (NSString*) createNetworkWithName:(NSString*)arg1 withUsers:(NSArray*)arg2 {
    return @"";
}

@end
