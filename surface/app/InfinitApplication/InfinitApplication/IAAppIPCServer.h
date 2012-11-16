//
//  IAAppIPCServer.h
//  InfinitApplication
//
//  Created by infinit on 11/15/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol IAAppIPCProtocol <NSObject>

- (void)sendUserToken:(NSString*)token;

@end

/**
 * @brief The exposed interface for the main application.
 *
 * The main application is responsible for preferences and notifications
 * panel. It does not use any gap state directly (which is in FinderWindow
 * component) but is updated through this interface.
 */
@interface IAAppIPCServer : NSObject <IAAppIPCProtocol>

- (id)initWithAppDelegate:(id)app;

@end
