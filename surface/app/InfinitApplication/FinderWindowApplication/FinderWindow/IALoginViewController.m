//
//  IALoginViewController.m
//  FinderWindowApplication
//
//  Created by infinit on 10/29/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IALoginViewController.h"

#import "IAGapState.h"

@interface IALoginViewController ()

@property(retain) IBOutlet NSTextFieldCell* login;
@property(retain) IBOutlet NSSecureTextFieldCell* password;
@property(retain) IBOutlet NSButton* login_button;
@property(retain) IBOutlet NSButton* register_button;
@property(retain) IBOutlet NSTextField* error_message;

-(IBAction) doLogin:(id)sender;

@end

@implementation IALoginViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
    }
    return self;
}


- (void)awakeFromNib
{
    [self.error_message setStringValue:@""];
}


// Login button
-(IBAction) doLogin:(id)sender
{
    NSString* login = [self.login stringValue];
    NSString* password = [self.password stringValue];
    NSString* device_name = @"TODO";
    [self.login_button setHidden:YES];
    [self.login setEnabled:NO];
    [self.password setEnabled:NO];
    [[IAGapState instance]              login:login withPassword:password
                                andDeviceName:device_name
                              performSelector:@selector(_login_callback:)
                                     onObject:self];
}


// Login callback
-(void) _login_callback:(IAGapOperationResult*)result
{
    NSLog(@"Login result: %d", [result error]);
    
    [self.login_button setHidden:NO];
    [self.login setEnabled:YES];
    [self.password setEnabled:YES];
    
    if (![result success])
        [self.error_message setStringValue:@"Wrong login / password"];
    [[NSNotificationCenter defaultCenter] postNotificationName:IA_GAP_EVENT_LOGIN_OPERATION
                                                        object:result];
}

@end
