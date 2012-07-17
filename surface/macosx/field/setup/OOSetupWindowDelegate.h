//
//  OOSetupWindowsDelegate.h
//  Surface
//
//  Created by Charles Guillot on 29/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/CoreAnimation.h>

#import "OOSetupView.h"

extern NSString *OOUserLoggedNotification;
extern NSString *OOUserUnLoggedNotification;

@interface OOSetupWindowDelegate : NSObject<NSWindowDelegate> {
    // Triggers //
    BOOL                            isLogged;
    
    // Commons //
    IBOutlet NSWindow               *window;
    IBOutlet OOSetupView            *currentView;
    IBOutlet OOSetupView            *welcomeView;
    IBOutlet OOSetupView            *loginView;
    IBOutlet OOSetupView            *registerView;
    IBOutlet OOSetupView            *loggedView;
    IBOutlet NSMatrix               *welcomeMatrix;
    
    // Login view //
    IBOutlet NSTextField            *loginViewComputerName;
    IBOutlet NSTextField            *loginViewEmail;
    IBOutlet NSSecureTextField      *loginViewPassword;
    IBOutlet NSTextField            *loginViewError;
    
    
    // Register view //
    IBOutlet NSTextField            *registerViewComputerName;
    IBOutlet NSTextField            *registerViewEmail;
    IBOutlet NSTextField            *registerViewFullName;
    IBOutlet NSSecureTextField      *registerViewPassword;
    IBOutlet NSSecureTextField      *registerViewPasswordVerif;
    IBOutlet NSTextField            *registerViewActivationCode;
    IBOutlet NSButtonCell           *registerViewTermOfService;
    IBOutlet NSButtonCell           *registerViewContinue;
    IBOutlet NSTextField            *registerViewError;
    
    // Thank you page //
    
    CATransition *transition;
}

@property BOOL                      isLogged;

@property(retain)NSWindow           *window;
@property(retain)OOSetupView        *currentView;
@property(retain)OOSetupView        *welcomeView;
@property(retain)OOSetupView        *loginView;
@property(retain)OOSetupView        *registerView;
@property(retain)OOSetupView        *loggedView;
@property(retain)NSMatrix           *welcomeMatrix;

// Login view //
@property(retain)NSTextField        *loginViewComputerName;
@property(retain)NSTextField        *loginViewEmail;
@property(retain)NSSecureTextField  *loginViewPassword;
@property(retain)NSTextField        *loginViewError;

// Register view //
@property(retain)NSTextField        *registerViewComputerName;
@property(retain)NSTextField        *registerViewEmail;
@property(retain)NSTextField        *registerViewFullName;
@property(retain)NSSecureTextField  *registerViewPassword;
@property(retain)NSSecureTextField  *registerViewPasswordVerif;
@property(retain)NSTextField        *registerViewActivationCode;
@property(retain)NSButtonCell       *registerViewTermOfService;
@property(retain)NSButtonCell       *registerViewContinue;
@property(retain)NSTextField        *registerViewError;

- (void)loginResult:(NSNumber *)arg1;
- (void)registerResult:(NSNumber *)arg1;

@end
