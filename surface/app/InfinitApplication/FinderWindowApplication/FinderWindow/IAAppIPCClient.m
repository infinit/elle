//
//  IAAppIPCClient.m
//  FinderWindow
//
//  Created by infinit on 11/15/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAAppIPCClient.h"

#import <ServiceManagement/ServiceManagement.h>
#import <Foundation/NSConnection.h>

#import "IAGapState.h"
#import "../../InfinitApplication/IAAppIPCServer.h"

@implementation IAAppIPCClient

+ (void) sendUserInfos
{
    id<IAAppIPCProtocol> proxy = (id<IAAppIPCProtocol>)[
         NSConnection rootProxyForConnectionWithRegisteredName:@"io.infinit.InfinitApplication"
                                                          host:nil
    ];
    if (proxy == nil)
        NSLog(@"Cannot connect !!");
    [proxy sendUserToken:[[IAGapState instance] token]];
}

@end
