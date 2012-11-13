//
//  InfinitestAppDelegate.h
//  Infinitest
//
//  Created by Simon Peccaud on 3/20/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "RootViewController.h"
#import "TopNotifViewController.h"

@interface InfinitAppDelegate : NSObject <UIApplicationDelegate>

@property (nonatomic) IBOutlet UIWindow *window;
@property (nonatomic) IBOutlet UITabBarController *tabBarController;
@property (nonatomic) IBOutlet UINavigationController *oNavigationController;
@property (strong, nonatomic) IBOutlet RootViewController   *oRootViewControler;
@property (strong) UIImageView *loginBackView;
@property (strong) UIImageView *rootBackView;
@property (strong) TopNotifViewController*      topNotifCtrl;

- (void) putRootBackView;
- (void) dismissRootBackView;

@end
