//
//  InfinitestAppDelegate.m
//  Infinitest
//
//  Created by Simon Peccaud on 3/20/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import "InfinitAppDelegate.h"

@implementation InfinitAppDelegate

@synthesize window;
@synthesize tabBarController;
@synthesize oNavigationController;
@synthesize oRootViewControler;
@synthesize loginBackView;
@synthesize rootBackView;
@synthesize topNotifCtrl;

- (void) putRootBackView {
    if (!rootBackView) {
        if ([UIScreen mainScreen].bounds.size.height == 568)   // iPhone 5 ++
            rootBackView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"tableview-background-568h@2x.png"]];
        else
            rootBackView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"tableview-background.png"]];

        [rootBackView setFrame:self.window.frame];
    }
    [self.window insertSubview:rootBackView aboveSubview:loginBackView];
}

- (void) dismissRootBackView {
        [self.rootBackView removeFromSuperview];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    if ([UIScreen mainScreen].bounds.size.height == 568)   // iPhone 5 ++
        loginBackView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"Default-568h@2x.png"]];
    else
        loginBackView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"Default.png"]];

    [loginBackView setFrame:self.window.frame];
    [self.window addSubview:loginBackView];
    
    self.oNavigationController.view.backgroundColor = [UIColor clearColor];
    self.window.rootViewController = self.oNavigationController;
    [self.window makeKeyAndVisible];

    self.oRootViewControler.appView = self.oNavigationController.view;
    self.topNotifCtrl = [[TopNotifViewController alloc] initWithView:self.oNavigationController];
    
    [[NSFileManager defaultManager] createDirectoryAtPath:[NSHomeDirectory() stringByAppendingPathComponent:ROOT_FILE_DIR] withIntermediateDirectories:FALSE attributes:nil error:nil];

    NSURL   *url = [[NSURL alloc] initFileURLWithPath:[NSHomeDirectory() stringByAppendingPathComponent:ROOT_FILE_DIR]];       // Stuff for disabling iCloud backup for "Documents/Infinit" local file folder
    BOOL success = [url setResourceValue: [NSNumber numberWithBool: YES]
                                  forKey: NSURLIsExcludedFromBackupKey error: nil];
    if(!success){
        NSLog(@"Error excluding %@ from backup", url);
    }

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
     */
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    /*
     Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
     */
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    /*
     Called when the application is about to terminate.
     Save data if appropriate.
     See also applicationDidEnterBackground:.
     */
}

-(BOOL)application:(UIApplication *)application 
           openURL:(NSURL *)url
 sourceApplication:(NSString *)sourceApplication
        annotation:(id)annotation {    
    if (url != nil && [url isFileURL]) {
        [self.oRootViewControler handleDocumentOpenURL:url];
    }    
    return YES;
}

@end
