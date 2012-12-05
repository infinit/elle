//
//  IAUserSearchBar.m
//  FinderWindowApplication
//
//  Created by infinit on 11/5/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAUserSearchBar.h"
#import "IAMainViewController.h"

@interface IAUserSearchBar ()

@property(retain) IBOutlet IAMainViewController* main_controller;

@end

@implementation IAUserSearchBar

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

//- (void)drawRect:(NSRect)dirtyRect
//{
//    // Drawing code here.
//}

- (NSString*)getUser
{
    return [self stringValue];
}

- (void)textDidChange:(NSNotification *)notification
{
    [self.main_controller refresh];
    static BOOL completing = false;
    if (!completing)
    {
        NSTextView * fieldEditor = [[notification userInfo] objectForKey:@"NSFieldEditor"];
        completing = true;
        [fieldEditor complete:nil];
        completing = false;
    }
}

- (BOOL)_isValidEmail
{
    NSString* regex =
    @"(?:[a-z0-9!#$%\\&'*+/=?\\^_`{|}~-]+(?:\\.[a-z0-9!#$%\\&'*+/=?\\^_`{|}"
    @"~-]+)*|\"(?:[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x21\\x23-\\x5b\\x5d-\\"
    @"x7f]|\\\\[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])*\")@(?:(?:[a-z0-9](?:[a-"
    @"z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\\[(?:(?:25[0-5"
    @"]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-"
    @"9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x21"
    @"-\\x5a\\x53-\\x7f]|\\\\[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])+)\\])";
    NSPredicate* predicate = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", regex];
    NSString* str = [[self stringValue] lowercaseString];
    return [predicate evaluateWithObject:str];
}

- (BOOL)isValid
{
    return [self _isValidEmail];
}

- (void) controlTextDidChange: (NSNotification *)note {
    NSLog(@"BIETIBIETIBIET");
    //    NSTextView * fieldEditor = [[note userInfo] objectForKey:@"NSFieldEditor"];
    //
    //    if (!isCompleting) {
    //        isCompleting = YES;
    //        [fieldEditor complete:nil];
    //        isCompleting = NO;
    //    }
}

@end
