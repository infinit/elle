//
//  LoginViewController.m
//  Infinitest
//
//  Created by Simon Peccaud on 5/4/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "LoginViewController.h"
#import "InfinitAppDelegate.h"

@interface LoginViewController ()
@end

@implementation LoginViewController

@synthesize oLoginField;
@synthesize oPassField;
@synthesize oRememberSwitch;
@synthesize oRememberLbl;
@synthesize oNetworkErrorImageView;
@synthesize oNetworkErrorLbl;
@synthesize oSignUpButton;
@synthesize oDeleteFilesLbl;
@synthesize oDeleteFilesConfirmButton;
@synthesize oDeleteFilesCancelButton;
@synthesize userMail;
@synthesize pswd;
@synthesize oSpinner;

- (IBAction)oSignUp:(id)sender {
    SignUpViewController *signUpViewController = [[SignUpViewController alloc] initWithNibName:@"SignUpViewController" bundle:NULL];

    self.view.backgroundColor = [UIColor blackColor];

    signUpViewController.loginView = self.view;
    signUpViewController.modalTransitionStyle = UIModalTransitionStylePartialCurl;
    [self presentModalViewController:signUpViewController animated:YES];
    UIResponder* nextResponder = [signUpViewController.view viewWithTag:1];

    if (nextResponder) {
        [nextResponder becomeFirstResponder];
    }
}

- (IBAction)oGoNextResponder:(id)sender {
    UITextField*    txtField = (UITextField*) sender;
    NSInteger      nextTag = txtField.tag + 1;
    UIResponder* nextResponder = [txtField.superview viewWithTag:nextTag];

    if (nextResponder) {
        // Found next responder, so set it.
        [((UITextField*) nextResponder) setEnabled:TRUE];
        [nextResponder becomeFirstResponder];
    } else {
        // Not found, so remove keyboard.
        [txtField resignFirstResponder];
    }
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    if ([SignUpViewController isValidEmail:self.oLoginField.text])
        return TRUE;

    self.oPassField.text = @"";
    [self.oPassField setEnabled:FALSE];
    [self.oLoginField becomeFirstResponder];
    return FALSE;
}

- (NSString*) loginRequest:(NSDictionary*) dictionary {

    self.oPassField.text = @"";
    NSString *JSON = [dictionary JSONString];

    // NSLog(@"SENT <%@>", JSON);

    NSData *theData = [JSON dataUsingEncoding:NSUTF8StringEncoding];
    NSURL *url = [NSURL URLWithString:@"http://infinit.im:12345/user/login"];

    NSString *postLength = [NSString stringWithFormat:@"%d", [theData length]];

    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];
    NSError *error = NULL;
    NSURLResponse *response = nil;

    [request setURL:url];
    [request setHTTPMethod:@"POST"];
    [request setTimeoutInterval:10];
    [request setValue:postLength forHTTPHeaderField:@"Content-Length"];
    [request setCachePolicy:NSURLRequestReloadIgnoringCacheData];
    [request setHTTPBody:theData];

    NSData *result = [NSURLConnection sendSynchronousRequest:request
                                           returningResponse:&response error:&error];

    NSString *resultString = [[NSString alloc] initWithData:result encoding:NSUTF8StringEncoding];
   // NSLog(@"\n\t=> %@ \n\t=> %@ \n\t=> %@", resultString, error.userInfo, response.description);

    NSDictionary*   dico = [resultString objectFromJSONString];

    if (([resultString length] != 0) && (CFBooleanGetValue((__bridge CFBooleanRef)([dico valueForKey:@"success"])))) {  // Online authentication suceeded
        token = [dico valueForKey:@"token"];
        NSLog(@"!%@!", token);
    }
    else {
        token = nil;
        NSLog(@"Nope");
    }

    return resultString;
}

- (IBAction)oLogin:(id)sender {
    if (sender) { // regular login (not remember user <=> not triggered from rootView)
        self.userMail = self.oLoginField.text;
        self.pswd = [InfinitCrypto cryptEmail:self.oLoginField.text Password:self.oPassField.text];
    }

    NSDictionary *dictionary = [[NSDictionary alloc] initWithObjectsAndKeys:
                                self.userMail,   @"email",
                                self.pswd,   @"password",
                                nil];

    NSString            *path = [[NSHomeDirectory() stringByAppendingPathComponent:@"Documents"] stringByAppendingPathComponent:@"InfinitUInfo.plist"];
    NSMutableDictionary *plistDico = nil;
    if ([[NSFileManager defaultManager] fileExistsAtPath:path])
        plistDico = [[NSMutableDictionary alloc] initWithContentsOfFile: path];

    if (plistDico) {
        NSLog(@">> EXIST : %@", path);
        NSLog(@">> M : %@        P : %@", [plistDico objectForKey:@"rememberM"], [plistDico objectForKey:@"rememberP"]);
    }
    else
        NSLog(@">> NOPE : %@", path);

    NSString*   resultString = [self loginRequest:dictionary];

    if ([resultString length] == 0) {     // Offline mode
        if (!plistDico ||                                                               // plistDico doesn't exist, cannot authenticate or
            !([[plistDico objectForKey:@"rememberM"] isEqualToString:self.userMail] &&
              [[plistDico objectForKey:@"rememberP"] isEqualToString:self.pswd])) {      // file exists but credentials doesn't correspond to local file
                [self showAuthError:TRUE offline:TRUE];
            }
        else {                                                                          // offline authentication suceeded, show network error then files
           // [self showNetworkError:TRUE];
            [self pushRootView];
           // [NSTimer    scheduledTimerWithTimeInterval:2.0    target:self    selector:@selector(pushRootView) userInfo:nil repeats:NO];
        }
    }
    else                                // Online mode
    {
        dictionary = [resultString objectFromJSONString];

        if (CFBooleanGetValue((__bridge CFBooleanRef)([dictionary valueForKey:@"success"]))) {  // Online authentication suceeded
            //  token = [dictionary valueForKey:@"token"];

            // [self showAuthError:FALSE offline:FALSE];

            if (plistDico &&
                ![[plistDico objectForKey:@"rememberM"] isEqualToString:@""] &&
                ![[plistDico objectForKey:@"rememberM"] isEqualToString:self.userMail])     // New user, delete local files warning
                [self showDeleteLocalFilesWarning:TRUE];
            else                                                                            // Same user, no warning, show files
                [self pushRootView];
        }
        else                                                                                // Online authentication failed
            [self showAuthError:TRUE offline:FALSE];
    }
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
       /* UIImage* infinitBack;

        NSLog(@":%f:", [UIScreen mainScreen].bounds.size.height);

        if ([UIScreen mainScreen].bounds.size.height == 568)   // iPhone 5 ++
            infinitBack = [UIImage imageNamed:@"Default-568h@2x.png"];
        else
           infinitBack = [UIImage imageNamed:@"Default@2x.png"];

        UIGraphicsBeginImageContext(self.view.bounds.size);

        CGContextRef context = UIGraphicsGetCurrentContext();
        CGContextTranslateCTM(context, 0.0, self.view.bounds.size.height);
        CGContextScaleCTM(context, 1.0, -1.0);

        CGContextDrawImage(context, CGRectMake(0.0f, 0.0f, self.view.bounds.size.width, self.view.bounds.size.height), infinitBack.CGImage);

        UIImage* scaledImage = UIGraphicsGetImageFromCurrentImageContext();

        UIGraphicsEndImageContext();*/

        //self.view.backgroundColor = [UIColor colorWithPatternImage:infinitBack];

//        self.view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"Background"]];

        self.oLoginField.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"textfield-back.png"]];
        self.oPassField.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"textfield-back.png"]];

        self.oRememberSwitch.transform = CGAffineTransformMakeScale(0.75, 0.75);

        [[NSNotificationCenter defaultCenter]   addObserver:self
                                                   selector:@selector(dismissSignUpView:)
                                                       name:@"dismissSignUpView"
                                                     object:nil];
        [self.oDeleteFilesConfirmButton setBackgroundImage:[[UIImage imageNamed:@"grey-button.png"] stretchableImageWithLeftCapWidth:10.0 topCapHeight:0.0] forState:UIControlStateNormal];
        [self.oDeleteFilesCancelButton setBackgroundImage:[[UIImage imageNamed:@"grey-button.png"] stretchableImageWithLeftCapWidth:10.0 topCapHeight:0.0] forState:UIControlStateNormal];
    }
    return self;
}

- (IBAction)dismissSignUpView:(id)sender {
    SignUpViewController    *signUpViewController = (SignUpViewController *) self.presentedViewController;

    self.oLoginField.text = signUpViewController.oEmail.text;
    [self.oPassField setEnabled:TRUE];

    [self.presentedViewController dismissModalViewControllerAnimated:YES];
    [self.oPassField becomeFirstResponder];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];

    [self showAuthForm:TRUE animated:TRUE];
    [self.view setBackgroundColor:[UIColor clearColor]];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void) pushRootView {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"didLogIn" object:self];

    InfinitAppDelegate *infinitAppDelegate = [[UIApplication sharedApplication] delegate];
    [infinitAppDelegate putRootBackView];

    [self.oRememberSwitch setOn:FALSE];
    self.oLoginField.text = @"";
    [self.oPassField setEnabled:FALSE];
//    [self showNetworkError:FALSE];
}

- (IBAction)oDeleteLocalFilesAndLogin:(id)sender {  // Delete all local files (trigered by user change detection)
    NSString*           fileURL;
    NSError*            error;
    NSString*           filesDirectory = [NSHomeDirectory() stringByAppendingPathComponent:ROOT_FILE_DIR];
    NSFileManager*      fileMgr = [NSFileManager defaultManager];
    NSArray*            array = [fileMgr contentsOfDirectoryAtPath:filesDirectory error:&error];
    NSEnumerator* nextFile = [array objectEnumerator];

    while (fileURL = [nextFile nextObject])
        [fileMgr removeItemAtPath:[filesDirectory stringByAppendingPathComponent:fileURL] error:&error];

    self.userMail = self.oLoginField.text;
    self.pswd = self.oPassField.text;

    [self pushRootView];
}

- (IBAction)oDismissLocalFilesWarning:(id)sender {
    [self showDeleteLocalFilesWarning:FALSE];

    [self logoutRequest];
}

- (void) showNetworkError:(BOOL) set {
    CGFloat alpha = 0;
    int offset = -65;
    int duration = set * 0.35;

    if (set) {
        if (self.oNetworkErrorLbl.alpha)       // Already set, exit
            return;
        alpha = 1;
        offset = -offset;
    }
    else if (!self.oNetworkErrorLbl.alpha)       // Already hidden, exit
        return

    [self showAuthForm:!set animated:FALSE];

    [UIView animateWithDuration:duration animations:^() {
        [self showAuthForm:!set animated:NO];

        if (set) {
            [self.oSpinner stopAnimating];
            [self.oSpinner setHidden:TRUE];
        }

        [self.oNetworkErrorImageView setAlpha:alpha];

        [self.oNetworkErrorLbl setAlpha:alpha];
        [self.oNetworkErrorLbl setFrame:CGRectMake(20, self.oNetworkErrorLbl.frame.origin.y + offset, 289, 67)];
    }];
}

- (void) showDeleteLocalFilesWarning:(BOOL) set {
    [self showAuthForm:!set animated:FALSE];

    if (set) {
        [self.oSpinner stopAnimating];
        [self.oSpinner setHidden:TRUE];
    }

    [self.oDeleteFilesLbl setHidden:!set];
    [self.oDeleteFilesConfirmButton setHidden:!set];
    [self.oDeleteFilesCancelButton setHidden:!set];
}

- (void) showSpinner:(BOOL) set {
    [self.oLoginField setEnabled:!set];
    [self.oPassField setEnabled:!set];

    if (set) {
        [self.oSpinner startAnimating];
    }
    else
        [self.oSpinner stopAnimating];
    [self.oSpinner setHidden:!set];
}

- (IBAction) oPassDidEndOnExit:(id)sender {
    [self.oPassField resignFirstResponder];
    [self showSpinner:TRUE];

    [NSTimer    scheduledTimerWithTimeInterval:0.5    target:self    selector:@selector(oLogin:) userInfo:nil repeats:NO];
}

- (void) showAuthForm:(BOOL) set animated:(BOOL) animated {
    CGFloat alpha = 0;
    int offset = -65;
    CGFloat duration = 0;

    if (set) {
        if (self.oLoginField.alpha)       // Already set, exit
            return;
        alpha = 1;
        offset = -offset;
        if (animated) {
            [self.oLoginField setFrame:CGRectMake(self.oLoginField.frame.origin.x,
                                                  46 - offset,
                                                  self.oLoginField.frame.size.width,
                                                  self.oLoginField.frame.size.height) ];
            [self.oPassField setFrame:CGRectMake(self.oPassField.frame.origin.x,
                                                  76 - offset + 30,
                                                  self.oPassField.frame.size.width,
                                                  self.oPassField.frame.size.height) ];
        }
    }
    else if (!self.oLoginField.alpha)       // Already hidden, exit
        return;

    if (animated)
        duration = 0.55;

    [UIView animateWithDuration:duration animations:^() {
        // [self.oLoginField setHidden:!set];
        [self.oLoginField setAlpha:alpha];
        [self.oLoginField setFrame:CGRectMake(self.oLoginField.frame.origin.x,
                                              46,
                                              self.oLoginField.frame.size.width,
                                              self.oLoginField.frame.size.height) ];
        [self.oLoginField setEnabled:set];

        // [self.oPassField setHidden:!set];
        [self.oPassField setAlpha:alpha];
        [self.oPassField setFrame:CGRectMake(self.oPassField.frame.origin.x,
                                             76,
                                             self.oPassField.frame.size.width,
                                             self.oPassField.frame.size.height) ];
        [self.oRememberLbl setAlpha:alpha];
        [self.oRememberSwitch setAlpha:alpha];
        [self.oSignUpButton setAlpha:alpha];
    }];
}

- (void) showAuthError:(BOOL) set offline:(BOOL) offline {
    if (set) {
        [self showSpinner:FALSE];

        [self.oPassField resignFirstResponder];

        if (offline)
            [self.oPassField setPlaceholder:@"offline authentication failed"];
        else
            [self.oPassField setPlaceholder:@"authentication failed"];
        [self.oPassField setValue:[UIColor redColor]
                   forKeyPath:@"_placeholderLabel.textColor"];
    }
    else {
        [self.oPassField setPlaceholder:@"password"];
        [self.oPassField setValue:[UIColor lightGrayColor]
                       forKeyPath:@"_placeholderLabel.textColor"];
    }
}

- (void) logoutRequest {
    NSURL *url = [NSURL URLWithString:@"http://infinit.im:12345/user/logout"];

    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];
    [request setHTTPShouldHandleCookies:NO];
    NSError *error = NULL;
    NSURLResponse *response = nil;

    [request setURL:url];

    [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
    [request setValue:token forHTTPHeaderField:@"AUTHORIZATION"];

    [request setCachePolicy:NSURLRequestReloadIgnoringCacheData];

    /* NSData *result = */
    [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&error];

//    NSString *resultString = [[NSString alloc] initWithData:result encoding:NSUTF8StringEncoding];
    //NSLog(@"\n\t=> %@ \n\t=> %@ \n\t=> %@", resultString, error.userInfo, response.description);
}

- (IBAction)oResignKeyboard:(id)sender {
    [self.oLoginField resignFirstResponder];
    [self.oPassField resignFirstResponder];
}

- (void) viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];

    InfinitAppDelegate *infinitAppDelegate = [[UIApplication sharedApplication] delegate];
    [infinitAppDelegate dismissRootBackView];
    [self.view setBackgroundColor:[UIColor clearColor]];

}

@end
