//
//  IAClientGapState.m
//  InfinitApplication
//
//  Created by infinit on 11/15/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAClientGapState.h"

@implementation IAClientGapState
{
@private
    IAGapState* _instance;
}

- (id) _init
{
    self = [super init];
    if (self)
    {
        _instance = nil;
    }
    return self;
}

+ (IAClientGapState*) _instance
{
    static IAClientGapState* client_state = nil;
    if (client_state == nil)
        client_state = [[IAClientGapState alloc] _init];
    return client_state;
}

- (IAGapState*) _gap_instance
{
    assert(_instance != nil);
    return _instance;
}

+ (IAGapState*) gap_instance
{
    return [[self _instance] _gap_instance];
}

+ (void) initGapInstanceWithToken:(NSString*)token
{
    return [[self _instance] _initGapInstanceWithToken:token];
}

- (void) _initGapInstanceWithToken:(NSString*)token
{
    assert(_instance == nil);
}

+ (BOOL) ready
{
    return [[self _instance] _ready];
}

- (BOOL) _ready
{
    return _instance != nil && _instance.logged_in;
}

@end
