//
//  IAMainViewController.m
//  FinderWindowApplication
//
//  Created by infinit on 10/31/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAMainViewController.h"
#import "IADropFileView.h"
#import "IAUserSearchBar.h"
#import "IAGapState.h"

@interface IAMainViewController ()
{
    BOOL _locked;
}

@property(retain) IBOutlet IADropFileView* drop_box;
@property(retain) IBOutlet IAUserSearchBar* search_bar;
@property(retain) IBOutlet NSButton* send_button;

-(IBAction) sendFile:(id)sender;

@end

@implementation IAMainViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        _locked = false;
    }
    
    return self;
}

- (void)awakeFromNib
{
    NSLog(@"INIT %@", self.search_bar);
    [self.search_bar setFocusRingType:NSFocusRingTypeNone];
    [[self.search_bar cell] setPlaceholderString:@"Find friend or send via email"];
    [self refresh];
}

-(IBAction) sendFile:(id)sender
{
    _locked = true;
    [self refresh];
    [[IAGapState instance] sendFiles:self.drop_box.pending_files
                              toUser:[self.search_bar getUser]
                     performSelector:@selector(_onFileSent:)
                            onObject:self];
}

- (void) _onFileSent:(IAGapOperationResult*)result
{
    [self.drop_box reset];
    _locked = false;
    [self refresh];
}

- (void)refresh
{
    NSLog(@"Refresh !");
    [self.send_button setHidden:true];
    [self.search_bar setEnabled:true];
    
    if (!_locked && [[self drop_box] hasPendingFiles])
    {
        if ([self.search_bar isValid])
        {
            [self.send_button setEnabled:true];
            [self.send_button setHidden:false];
        }
        else
        {
            [self.send_button setEnabled:false];
            [self.send_button setHidden:true];
        }
    }
    else
    {
        [self.search_bar setEnabled:true];
        [self.send_button setEnabled:false];
    }
}

@end