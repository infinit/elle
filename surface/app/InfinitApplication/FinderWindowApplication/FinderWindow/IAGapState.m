//
//  IAGapState.m
//  FinderWindowApplication
//
//  Created by infinit on 10/29/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAGapState.h"

#import "gap.h"


// Block type to queue gap operation
typedef int(^gap_operation_t)(void);

@interface IAGapOperationResult ()
{
    gap_Status status;
}

@property gap_Status status;

- (id)initWithStatusCode:(gap_Status)status_code;

@end


@implementation IAGapOperationResult

@synthesize status;

- (id)initWithStatusCode:(gap_Status)status_code
{
    self.status = status_code;
    return self;
}

-(BOOL)success
{
    return self.status == gap_ok;
}

-(BOOL)error
{
    return self.status != gap_ok;
}

@end

@interface IAGapState ()
{
    gap_State* state;
}

@property gap_State* state;

@end

@implementation IAGapState

@synthesize state;

+ (IAGapState*) instance
{
    static IAGapState* instance = NULL;
    
    if (instance == NULL)
        instance = [[IAGapState alloc] init];
    if (instance == NULL)
        [NSException raise:@"bad_alloc" format:@"Cannot create a new gap state"];
    return instance;
}

-(id) init
{
    self = [super init];
    
    if (!self)
        return nil;
    
    self.state = gap_new();
    if (self.state == NULL)
        [NSException raise:@"bad_alloc" format:@"Cannot create a new gap state"];
    return self;
}


- (void)                login:(NSString*)login
                 withPassword:(NSString*)password
                andDeviceName:(NSString*)device_name
              performSelector:(SEL)selector
                     onObject:(id)object;
{
    [self _addOperation:^(void) {
        char* hash_password = gap_hash_password(self.state, [login UTF8String], [password UTF8String]);
        int res = gap_login(self.state, [login UTF8String], hash_password);
        gap_hash_free(hash_password);
        if (res == gap_ok)
            res = gap_set_device_name(self.state, [device_name UTF8String]);
        return res;
    } performSelector:selector onObject:object];
}

-(void) _addOperation:(gap_operation_t)operation
      performSelector:(SEL)selector
             onObject:(id)object
{
    [self addOperationWithBlock:^(void) {
            sleep(2); //XXX
        int result = operation();
        [object performSelectorOnMainThread:selector
                                 withObject:[[IAGapOperationResult alloc] initWithStatusCode:result]
                              waitUntilDone:NO];
    }];
}

@end
