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
@synthesize loginViewError;
// Register view //
@synthesize registerViewComputerName;
@synthesize registerViewEmail;
@synthesize registerViewFullName;
@synthesize registerViewPassword;
@synthesize registerViewPasswordVerif;
@synthesize registerViewTermOfService;
@synthesize registerViewContinue;


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
    [pref setObject:[self.loginViewEmail stringValue]  forKey:@"Email"];
    [pref setObject:[[self.loginViewPassword stringValue] sha1]  forKey:@"Password"];
    [pref setObject:[self.loginViewComputerName stringValue]  forKey:@"ComputerName"];
    [pref synchronize];
    [[OOPhone getInstance] loginWithEmail:[pref objectForKey:@"Email"]
                                 password:[pref objectForKey:@"Password"]
                          performSelector:@selector(loginResult:) 
                                forObject:self];
}

- (void)loginResult:(NSNumber *)arg1 {
    int error = [arg1 intValue];
    if (error > 0)
        [self.loginViewError setStringValue:@"Super mega error!!!!"];
    else {
        [transition setSubtype:kCATransitionFromRight];
        [self swapView:loggedView];
        
        self.isLogged = YES;
    }
}

// Register View
- (IBAction)acceptTermOfService:(id)sender {
    [transition setSubtype:kCATransitionFromLeft];
    [self.registerViewContinue setEnabled:([self.registerViewTermOfService state] == NSOnState)];
    
}

- (IBAction)registerViewGoBack:(id)sender
{
    [self swapView:welcomeView];
}

- (IBAction)registerViewContinue:(id)sender
{
    NSUserDefaults *pref;
    pref=[NSUserDefaults standardUserDefaults];
    [pref setObject:[self.registerViewFullName stringValue]  forKey:@"FullName"];
    [pref setObject:[self.registerViewEmail stringValue]  forKey:@"Email"];
    [pref setObject:[[self.registerViewPassword stringValue] sha1]  forKey:@"Password"];
    [pref setObject:[self.registerViewComputerName stringValue]  forKey:@"ComputerName"];
    [pref synchronize];
    const char* a = [[pref objectForKey:@"Password"] UTF8String];
    [[OOPhone getInstance] registerWithFullName:[pref objectForKey:@"FullName"]
                                          email:[pref objectForKey:@"Email"]
                                       password:[pref objectForKey:@"Password"]
                                     machineName:[pref objectForKey:@"ComputerName"]
                                performSelector:@selector(loginResult:) 
                                      forObject:self];
}

- (void)windowWillClose:(NSNotification *)notification
{
    if (self.isLogged) {
        [[NSNotificationCenter defaultCenter] postNotificationName:OOUserLoggedNotification object:self];
    } else {
        [[NSNotificationCenter defaultCenter] postNotificationName:OOUserUnLoggedNotification object:self];
    }
}
@end
