//
//  IARegisterViewController.m
//  FinderWindowApplication
//
//  Created by infinit on 10/31/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IARegisterViewController.h"
#import "IAGapState.h"

@interface IARegisterViewController ()

@property(retain) IBOutlet NSTextFieldCell* fullname;
@property(retain) IBOutlet NSTextFieldCell* login;

@property(retain) IBOutlet NSSecureTextFieldCell* password;
@property(retain) IBOutlet NSSecureTextFieldCell* password_confirm;

@property(retain) IBOutlet NSTextField* error_message;

- (IBAction)doRegister:(id)sender;

@end

@implementation IARegisterViewController

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

- (IBAction)doRegister:(id)sender
{
    [[IAGapState instance] register_:[self.login stringValue]
                        withFullname:[self.fullname stringValue]
                         andPassword:[self.password stringValue]
                       andDeviceName:@"TODO"
                     performSelector:@selector(onRegister)
                            onObject:self];
}

- (void) onRegister:(NSNotification*)notification
{
    if ([[notification object] success])
        NSLog(@"Successfully registered !");
    else
        NSLog(@"Couldn't register :(");
}
@end
