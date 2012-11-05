//
//  LoginViewController.h
//  Infinitest
//
//  Created by Simon Peccaud on 5/4/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SignUpViewController.h"
#import "InfinitCrypto.h"
//#import "InfinitAppDelegate.h"

@interface LoginViewController : UIViewController {
}

@property (strong) IBOutlet UITextField* oLoginField;
@property (strong) IBOutlet UITextField* oPassField;
@property (strong) IBOutlet UISwitch*    oRememberSwitch;
@property (strong) IBOutlet UILabel*     oRememberLbl;
@property (strong) IBOutlet UIImageView *oNetworkErrorImageView;
@property (strong) IBOutlet UILabel     *oNetworkErrorLbl;
@property (strong) IBOutlet UIButton    *oSignUpButton;
@property (strong) IBOutlet UILabel     *oDeleteFilesLbl;
@property (strong) IBOutlet UIButton    *oDeleteFilesConfirmButton;
@property (strong) IBOutlet UIButton    *oDeleteFilesCancelButton;
@property (strong) NSString             *userMail;
@property (strong) NSString             *pswd;
@property (strong) IBOutlet UIActivityIndicatorView *oSpinner;

- (IBAction)oLogin:(id)sender;
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil;
- (IBAction)oGoNextResponder:(id)sender;
- (void) pushRootView;
- (IBAction)oDeleteLocalFilesAndLogin:(id)sender;
- (IBAction)oDismissLocalFilesWarning:(id)sender;
- (IBAction)oResignKeyboard:(id)sender;
- (void) showNetworkError: (BOOL) set;
- (void) showAuthError:(BOOL) set offline:(BOOL) offline;
- (void) showDeleteLocalFilesWarning:(BOOL) set;
- (void) showSpinner:(BOOL) set;
- (IBAction) oPassDidEndOnExit:(id)sender;
- (void) showAuthForm:(BOOL) set animated:(BOOL) animated;
- (void) logoutRequest;
- (NSString*) loginRequest:(NSDictionary*) dictionary;

@end

NSString*    token;