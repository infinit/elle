//
//  OOSetupWindowsDelegate.m
//  Surface
//
//  Created by Charles Guillot on 29/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOSetupWindowDelegate.h"
#import <AddressBook/AddressBook.h>
#import "Phone/OOPhone.h"
#import "NSString+Hashes.h"


NSString *OOUserLoggedNotification = @"OOUserLoggedNotification";
NSString *OOUserUnLoggedNotification = @"OOUserUnLoggedNotification";


@implementation OOSetupWindowDelegate

@synthesize isLogged;

@synthesize currentView;
@synthesize welcomeView;
@synthesize registerView;
@synthesize loginView;
@synthesize loggedView;
@synthesize window;
@synthesize welcomeMatrix;
// Login view //
@synthesize loginViewEmail;
@synthesize loginViewPassword;
@synthesize loginViewComputerName;
@synthesize loginViewRememberMe;
@synthesize loginViewError;
// Register view //
@synthesize registerViewComputerName;
@synthesize registerViewEmail;
@synthesize registerViewFullName;
@synthesize registerViewPassword;
@synthesize registerViewPasswordVerif;
@synthesize registerViewActivationCode;
@synthesize registerViewTermOfService;
@synthesize registerViewRememberMe;
@synthesize registerViewContinue;
@synthesize registerViewError;

- (void)awakeFromNib
{
    NSView *contentView = [[self window] contentView];
    [contentView setWantsLayer:YES];
    [contentView addSubview:[self currentView]];
    
    transition = [CATransition animation];
    [transition setType:kCATransitionPush];
    [transition setSubtype:kCATransitionFromLeft];
    
    NSDictionary *ani = [NSDictionary dictionaryWithObject:transition forKey:@"subviews"];
    [contentView setAnimations:ani];
    [[window contentView] setWantsLayer:YES];
    [[window contentView] layer].contents = [NSImage imageNamed:@"setup-bg.png"];
    
    // Computer Name
    [self.loginViewComputerName setStringValue:[[NSHost currentHost] localizedName]];
    [self.registerViewComputerName setStringValue:[[NSHost currentHost] localizedName]];
    // User Name
    [self.registerViewFullName setStringValue:NSUserName()];
    // Email addresse
    ABPerson *aPerson = [[ABAddressBook sharedAddressBook] me];
    ABMultiValue *emails = [aPerson valueForProperty:kABEmailProperty];
    if([emails count] > 0) {
        [self.loginViewEmail setStringValue:[emails valueAtIndex:0]];
        [self.registerViewEmail setStringValue:[emails valueAtIndex:0]];
    }
    // Focus
    [self.window setLevel:NSFloatingWindowLevel];
    
    // Error label
    [self.registerViewError setStringValue:@""];
    [self.loginViewError setStringValue:@""];
    
    //isLogged
    self.isLogged = NO;
}

- (void)swapView:(OOSetupView*)newView
{
    if (!currentView) {
        currentView = newView;
        return;
    }
    NSView *contentView = [self.window contentView];
    //[self.window setContentView:newView];
    [[contentView animator] replaceSubview:currentView with:newView];
    currentView = newView;
}

- (IBAction)welcomeViewContinue:(id)sender
{
    [transition setSubtype:kCATransitionFromRight];
    if ([welcomeMatrix selectedRow] == 0) {
        [self swapView:registerView];
    } else {
        [self swapView:loginView];
    }
}

// Login View
- (IBAction)loginViewGoBack:(id)sender
{
    [transition setSubtype:kCATransitionFromLeft];
    [self swapView:welcomeView];
}

- (IBAction)loginViewContinue:(id)sender
{
    NSUserDefaults *pref;
    pref=[NSUserDefaults standardUserDefaults];
    NSString* hashPassword = [[OOPhone getInstance] getHashPasswordWithEmail:[self.loginViewEmail stringValue] andClearPassword:[self.loginViewPassword stringValue]];
    [pref setObject:[self.loginViewEmail stringValue]  forKey:@"Email"];
    [pref setObject:hashPassword  forKey:@"Password"];
    [pref setObject:[self.loginViewComputerName stringValue]  forKey:@"ComputerName"];
    [pref setObject:[self.loginViewRememberMe stringValue]  forKey:@"RememberMe"];
    [pref synchronize];
    NSLog(@"User try to login");
    [[OOPhone getInstance] loginWithEmail:[pref objectForKey:@"Email"]
                                 password:[pref objectForKey:@"Password"]
                              machineName:[pref objectForKey:@"ComputerName"]
                          performSelector:@selector(loginResult:) 
                                forObject:self];
}

- (void)loginResult:(NSNumber *)arg1 {
    int error = [arg1 intValue];
    if (error != 0) {
        NSLog(@"User login failled");
        [self.loginViewError setStringValue:@"An error occurred..."];
    }
    else {
        NSLog(@"User login succeed");
        [transition setSubtype:kCATransitionFromRight];
        [self swapView:loggedView];
        
        self.isLogged = YES;
        [self notififyApplicationIfUserIsLogged];
        [[self window] close];
        NSLog(@"Close setup windows");
    }
}

// Register View
- (IBAction)acceptTermOfService:(id)sender {
    [transition setSubtype:kCATransitionFromLeft];
    [self.registerViewContinue setEnabled:([self.registerViewTermOfService state] == NSOnState)];
    
}

- (IBAction)registerViewGoBack:(id)sender
{
    [transition setSubtype:kCATransitionFromLeft];
    [self swapView:welcomeView];
}

- (IBAction)registerViewContinue:(id)sender
{
    NSUserDefaults *pref;
    pref=[NSUserDefaults standardUserDefaults];
    NSString* hashPassword = [[OOPhone getInstance] getHashPasswordWithEmail:[self.registerViewEmail stringValue] andClearPassword:[self.registerViewPassword stringValue]];
    [pref setObject:[self.registerViewFullName stringValue] forKey:@"FullName"];
    [pref setObject:[self.registerViewEmail stringValue] forKey:@"Email"];
    [pref setObject:hashPassword forKey:@"Password"];
    [pref setObject:[self.registerViewComputerName stringValue] forKey:@"ComputerName"];
    [pref setObject:[self.registerViewActivationCode stringValue] forKey:@"ActivationCode"];
    [pref setObject:(([self.registerViewTermOfService state] == NSOnState) ? @"YES" : @"NO") forKey:@"RememberMe"];
    [pref synchronize];
    [[OOPhone getInstance] registerWithFullName:[pref objectForKey:@"FullName"]
                                          email:[pref objectForKey:@"Email"]
                                       password:[pref objectForKey:@"Password"]
                                    machineName:[pref objectForKey:@"ComputerName"]
                                 activationCode:[pref objectForKey:@"ActivationCode"]
                                performSelector:@selector(registerResult:) 
                                      forObject:self];
}

- (void)registerResult:(NSNumber *)arg1 {
    int error = [arg1 intValue];
    if (error != 0)
        [self.registerViewError setStringValue:@"An error occurred..."];
    else {
        [transition setSubtype:kCATransitionFromRight];
        [self swapView:loggedView];
        
        self.isLogged = YES;
        [self notififyApplicationIfUserIsLogged];
        [[self window] close];
    }
}

- (void)notififyApplicationIfUserIsLogged {
    if (self.isLogged) {
        NSLog(@"Send succeed logged notification");
        [[NSNotificationCenter defaultCenter] postNotificationName:OOUserLoggedNotification object:self];
    } else {
        NSLog(@"Send unsuccessful logging notification");
        [[NSNotificationCenter defaultCenter] postNotificationName:OOUserUnLoggedNotification object:self];
    }
    
}

- (BOOL)windowShouldClose:(id)sender {
    [self notififyApplicationIfUserIsLogged];
    return YES;
}
@end
