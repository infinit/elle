//
//  SignUpViewController.m
//  Infinit
//
//  Created by Simon Peccaud on 7/27/12.
//  Copyright (c) 2012 infinit.io. All rights reserved.
//

#import "SignUpViewController.h"

// Private
@interface SignUpViewController ()
@property (strong)      UIColor                 *_lblColor;
@end

@implementation SignUpViewController

@synthesize loginView;
@synthesize oEmail;
@synthesize oName;
@synthesize oPassw;
@synthesize oEmailLbl;
@synthesize oNameLbl;
@synthesize oPasswLbl;
@synthesize oResultLbl;
@synthesize oSpinner;
@synthesize oNetworkErrorImageView;
@synthesize oNetworkErrorLbl;
@synthesize _lblColor;

- (IBAction)oSignUp:(id)sender {
    BOOL success = FALSE;
    NSInteger checkResult = 1;
    
    [self.oEmail setEnabled:FALSE];
    [self.oName setEnabled:FALSE];
    [self.oPassw setEnabled:FALSE];

    [self.oSpinner setHidden:FALSE];
    [self.oSpinner startAnimating];

    checkResult = [self checkInputUpToTag:3];
    
    if (checkResult == 0) {
        NSString* pswd = [InfinitCrypto cryptEmail:self.oEmail.text Password:self.oPassw.text];
        
        NSDictionary *dictionary = [[NSDictionary alloc] initWithObjectsAndKeys:
                                    self.oEmail.text,   @"email",
                                    self.oName.text ,   @"fullname",
                                    pswd,   @"password",
                                    @"bitebite",        @"activation_code",
                                    nil];

        NSString *JSON = [dictionary JSONString];
        NSData *theData = [JSON dataUsingEncoding:NSUTF8StringEncoding];
        NSURL *url = [NSURL URLWithString:@"http://infinit.im:12345/user/register"];

        NSString *postLength = [NSString stringWithFormat:@"%d", [theData length]];

        NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];
        NSError *error = NULL;
        NSURLResponse *response = nil;

        NSLog(@"%@", url);

        [request setURL:url];
        [request setHTTPMethod:@"POST"];
        [request setValue:postLength forHTTPHeaderField:@"Content-Length"];
        [request setCachePolicy:NSURLRequestReloadIgnoringCacheData];
        [request setHTTPBody:theData];

        NSData *result = [NSURLConnection sendSynchronousRequest:request
                                               returningResponse:&response error:&error];

        NSString *resultString = [[NSString alloc] initWithData:result encoding:NSUTF8StringEncoding];

        if (resultString.length > 0)
        {
            NSDictionary *resultsDictionary = [resultString objectFromJSONString];
                
            if (! CFBooleanGetValue((__bridge CFBooleanRef)([resultsDictionary valueForKey:@"success"]))) {
                if (([resultsDictionary  valueForKey:@"error"]) &&
                      ([[resultsDictionary  valueForKey:@"error"] isEqualToString:@"This email is already registered"])) {
                    self.oEmailLbl.textColor = [UIColor redColor];
                    self.oEmailLbl.text = @"Already taken :(";
                }
                success = TRUE;
            }
            else
                success = TRUE;
            
            NSLog(@"\n\t=> %@ \n\t=> %@ \n\t=> %@", resultString, error.userInfo, response.description);
        }
        else {
            [self.oNetworkErrorLbl setHidden:FALSE];
            [self.oNetworkErrorImageView setHidden:FALSE];
        }
    }
    
    [self.oSpinner stopAnimating];
    [self.oSpinner setHidden:TRUE];
    
    if (success) {
        [self.oEmail removeFromSuperview];
        [self.oName removeFromSuperview];
        [self.oPassw removeFromSuperview];
        [self.oEmailLbl removeFromSuperview];
        [self.oNameLbl removeFromSuperview];
        [self.oPasswLbl removeFromSuperview];
        self.oResultLbl.text = @"Welcome to Infinit !\nYou can now login.";
        [NSTimer    scheduledTimerWithTimeInterval:2.0    target:self    selector:@selector(dismissSignUpWindow:)    userInfo:nil repeats:NO];
    }
    else
    {
        [self.oEmail setEnabled:TRUE];
        [self.oName setEnabled:TRUE];
        [self.oPassw setEnabled:TRUE];

        UIResponder* nextResponder = nextResponder = [self.view viewWithTag:checkResult];
        if (nextResponder)
            [nextResponder becomeFirstResponder];
    }
}

- (NSInteger) checkInputUpToTag:(NSInteger) tag {
    if (![SignUpViewController isValidEmail:self.oEmail.text])
    {
        self.oEmailLbl.textColor = [UIColor redColor];
        self.oEmailLbl.text = @"Invalid email";
        [self.oName setEnabled:FALSE];
        self.oPassw.text = @"";
        [self.oPassw setEnabled:FALSE];
        return 1;
    }

    self.oEmailLbl.textColor = [UIColor orangeColor];
    self.oEmailLbl.text = @"Your email";
    [self.oName setEnabled:TRUE];
    
    if (tag == 1)
        return 0;
    
    if (self.oName.text.length < 3)
    {
        self.oNameLbl.textColor = [UIColor redColor];
        self.oNameLbl.text = @"Too short";
        self.oPassw.text = @"";
        [self.oPassw setEnabled:FALSE];
        return 2;
    }
    if (self.oName.text.length > 90)
    {
        self.oNameLbl.textColor = [UIColor redColor];
        self.oNameLbl.text = @"Too long";
        self.oPassw.text = @"";
        [self.oPassw setEnabled:FALSE];
        return 2;
    }

    self.oNameLbl.textColor = [UIColor orangeColor];
    self.oNameLbl.text = @"Full name";
    [self.oPassw setEnabled:TRUE];
    
    if (tag == 2)
        return 0;
    
    if (self.oPassw.text.length < 3)
    {
        self.oPasswLbl.textColor = [UIColor redColor];
        self.oPasswLbl.text = @"Too short";
        return 3;
    }
    if (self.oPassw.text.length > 90)
    {
        self.oPasswLbl.textColor = [UIColor redColor];
        self.oPasswLbl.text = @"Too long";
        return 3;
    }

    self.oPasswLbl.textColor = [UIColor orangeColor];
    self.oPasswLbl.text = @"Set a password";

    return 0;
}

+ (BOOL) isValidEmail:(NSString *)checkString
{
    BOOL stricterFilter = YES;
    NSString *stricterFilterString = @"[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}";
    NSString *laxString = @".+@.+\\.[A-Za-z]{2}[A-Za-z]*";
    NSString *emailRegex = stricterFilter ? stricterFilterString : laxString;
    NSPredicate *emailTest = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", emailRegex];
    return [emailTest evaluateWithObject:checkString];
}

- (IBAction)oGoNextResponder:(id)sender {
    UITextField*    txtField = (UITextField*) sender;
    NSInteger      nextTag = txtField.tag + 1;
    UIResponder* nextResponder = [txtField.superview viewWithTag:nextTag];

    if (nextResponder) {
        // Found next responder, so set it.
        [nextResponder becomeFirstResponder];
    } else {
        // Not found, so remove keyboard.
        [txtField resignFirstResponder];
    }
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    return ([self checkInputUpToTag:textField.tag] != textField.tag);
}

- (void) dismissSignUpWindow:(id)sender {
        [[NSNotificationCenter defaultCenter] postNotificationName:@"dismissSignUpView" object:self];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        UIImage* infinitBack = [UIImage imageNamed:@"gain-background.png"];
        
        UIGraphicsBeginImageContext(self.view.bounds.size);
        
        CGContextRef context = UIGraphicsGetCurrentContext();
        CGContextTranslateCTM(context, 0.0, self.view.bounds.size.height);
        CGContextScaleCTM(context, 1.0, -1.0);
        
        CGContextDrawImage(context, CGRectMake(0.0f, 0.0f, self.view.bounds.size.width, self.view.bounds.size.height), infinitBack.CGImage);
        
        UIImage* scaledImage = UIGraphicsGetImageFromCurrentImageContext();
        
        UIGraphicsEndImageContext();
        
        self.view.backgroundColor = [UIColor colorWithPatternImage:scaledImage];
        
       // self.oSpinner = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhite];
       // [self._spinner setFrame:self.oSignUpButton.frame];
        
        self.oResultLbl.textColor = [UIColor orangeColor];
    }
    return self;
}

- (IBAction)oEditingDidBegin:(id)sender {
    [self.oNetworkErrorLbl setHidden:TRUE];
    [self.oNetworkErrorImageView setHidden:TRUE];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void) doNothing {
}

- (void) viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    
    [UIView animateWithDuration:0.3 animations:^() {
        [self.loginView setBackgroundColor:[UIColor clearColor]];
    }];
}

@end
