//
//  TopNotifViewController.m
//  Infinit
//
//  Created by Simon Peccaud on 10/3/12.
//  Copyright (c) 2012 infinit.io. All rights reserved.
//

#import "TopNotifViewController.h"

@interface TopNotifViewController ()

@end

@implementation TopNotifViewController

@synthesize navigationCtrl;
@synthesize textLabel;
@synthesize isRevealed;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }

    return self;
}

- (id)initWithView:(UINavigationController*) aView {
    self.navigationCtrl = aView;

    self.view.frame = CGRectMake(0,
                                 self.navigationCtrl.navigationBar.frame.origin.y + self.navigationCtrl.navigationBar.frame.size.height,
                                 self.navigationCtrl.navigationBar.frame.size.width,
                                 20);
    self.view.backgroundColor = [UIColor redColor];
    self.view.alpha = 0;

    self.textLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.height)];

    self.textLabel.backgroundColor = [UIColor clearColor];
    self.textLabel.textColor = [UIColor whiteColor];
    self.textLabel.textAlignment = NSTextAlignmentCenter;

    [self.view addSubview:self.textLabel];
    self.isRevealed = FALSE;

    [[NSNotificationCenter defaultCenter]   addObserver:self
                                               selector:@selector(revealNotifBar:)
                                                   name:@"revealNotifBar"
                                                 object:nil];
    [[NSNotificationCenter defaultCenter]   addObserver:self
                                               selector:@selector(hideNotifBar:)
                                                   name:@"hideNotifBar"
                                                 object:nil];
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
}

- (void)revealNotifBar:(NSNotification*) notification {

    if (notification.object)
        self.textLabel.text = notification.object;
    else
        self.textLabel.text = @"";

    if (self.isRevealed)
        return;

    //[self.tblView setContentOffset:CGPointMake(0, -20) animated:YES];
 //   UIEdgeInsets tblInset = self.tblView.contentInset;
 //   tblInset.top = 20;

 //   [self.tblView setContentInset:tblInset];

    [self.navigationCtrl.view addSubview:self.view];

    [UIView animateWithDuration:1 animations:^{
        self.view.alpha = 1;
    }];
}

- (void)hideNotifBar:(NSNotification*) notification {
   // [self.tblView setContentOffset:CGPointMake(0, 0) animated:YES];

    [UIView animateWithDuration:0.25 animations:^{
        self.view.alpha = 0;
    }];

    [self.view removeFromSuperview];
    self.textLabel.text = @"";
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
