//
//  SignUpViewController.h
//  Infinit
//
//  Created by Simon Peccaud on 7/27/12.
//  Copyright (c) 2012 infinit.io. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "JSONKit.h"
#import "InfinitCrypto.h"

@interface SignUpViewController : UIViewController

@property UIView*   loginView;
@property (strong) IBOutlet UITextField *oEmail;
@property (strong) IBOutlet UITextField *oName;
@property (strong) IBOutlet UITextField *oPassw;
@property (strong) IBOutlet UILabel *oEmailLbl;
@property (strong) IBOutlet UILabel *oNameLbl;
@property (strong) IBOutlet UILabel *oPasswLbl;
@property (strong) IBOutlet UILabel *oResultLbl;
@property (strong) IBOutlet UIActivityIndicatorView *oSpinner;
@property (strong) IBOutlet UIImageView *oNetworkErrorImageView;
@property (strong) IBOutlet UILabel     *oNetworkErrorLbl;

- (NSInteger) checkInputUpToTag:(NSInteger) tag;
+ (BOOL) isValidEmail:(NSString *)checkString;
- (IBAction)oGoNextResponder:(id)sender;
- (BOOL)textFieldShouldReturn:(UITextField *)textField;
- (IBAction)oEditingDidBegin:(id)sender;

@end