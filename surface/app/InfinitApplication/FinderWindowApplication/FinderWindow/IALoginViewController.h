//
//  IALoginViewController.h
//  FinderWindowApplication
//
//  Created by infinit on 10/29/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface IALoginViewController : NSViewController

@property(retain) IBOutlet NSTextFieldCell* login;

@property(retain) IBOutlet NSSecureTextFieldCell* password;

@property(retain) IBOutlet NSButton* login_button;
@property(retain) IBOutlet NSButton* register_button;
@property(retain) IBOutlet NSTextField* error_message;

-(IBAction) doLogin:(id)sender;

@end
