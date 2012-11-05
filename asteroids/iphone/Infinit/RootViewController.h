//
//  RootViewController.h
//  Infinitest
//
//  Created by Simon Peccaud on 3/20/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "InfinitData.h"
#import "LoginViewController.h"
#import "PullRefreshTableViewController.h"
#import "JTRevealSidebarV2Delegate.h"
#import "UINavigationItem+JTRevealSidebarV2.h"
#import "UIViewController+JTRevealSidebarV2.h"
#import "RightSidebarDataSource.h"
#import "SettingsViewController.h"
#import "DriveInfoViewController.h"
#import "InfinitNetworks.h"
#import "TopNotifViewController.h"
#import <QuartzCore/QuartzCore.h>
#import <MessageUI/MessageUI.h>
#import <MessageUI/MFMailComposeViewController.h>

@interface RootViewController : PullRefreshTableViewController <UIDocumentInteractionControllerDelegate,
    UISearchBarDelegate,
    UIGestureRecognizerDelegate,
    JTRevealSidebarV2Delegate,
    UIImagePickerControllerDelegate,
    UINavigationControllerDelegate,
    MFMailComposeViewControllerDelegate> {
}

@property(strong, nonatomic) IBOutlet UITableView *oTableView;
@property(strong, nonatomic) IBOutlet UISearchBar *oSearchBar;
@property(strong, nonatomic) InfinitData*   data;
@property(strong, nonatomic, retain) LoginViewController *loginViewController;
@property(strong, nonatomic) NSString   *optionFileToSave;
@property(weak, nonatomic) RootViewController *nextRootViewController;
@property(strong, nonatomic) UITableViewCell    *offlineCell;
@property CGRect tableViewEditModeFrame;
@property CGRect tableViewInitialFrame;
@property UIView    *appView;

- (void) handleDidLogIn: (NSNotification *) notif;
- (void) handleDidLogOut: (NSNotification *) notif;
- (IBAction) pushLoginView: (id) sender;
- (void)tableView:(UITableView*)tableView willBeginEditingRowAtIndexPath:(NSIndexPath *)indexPath;
- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath;

- (void) hideSearchBar;
- (void) alignSearchBar;
- (void)searchBarTextDidBeginEditing:(UISearchBar *)searchBar;
- (void)searchBarCancelButtonClicked:(UISearchBar *)searchBar;

- (IBAction)oPresentFolderTextInput:(id)sender;

- (void)refreshHeader;
- (IBAction)handleSwipeLeft:(UIGestureRecognizer *)sender;
- (IBAction)handlePinch:(UIPinchGestureRecognizer *)sender;

- (IBAction)oToggleAddView:(id)sender;

-(IBAction)oPresentSingleShotView:(id)sender;
- (BOOL) startCameraControllerFromViewController: (UIViewController*) controller
                                   usingDelegate: (id <UIImagePickerControllerDelegate,
                                                   UINavigationControllerDelegate>) delegate;
- (void) imagePickerControllerDidCancel: (UIImagePickerController *) picker;
- (void) imagePickerController: (UIImagePickerController *) picker
 didFinishPickingMediaWithInfo: (NSDictionary *) info;

-(IBAction)oPresentPhotoLibrary:(id)sender;
-(IBAction)oPresentSendEmailFile:(id)sender;

- (void)mailComposeController:(MFMailComposeViewController*)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error;

- (void)handleDocumentOpenURL:(NSURL *)url;

- (void) revealEditBar;
- (void) hideEditBar;

- (IBAction)oHandleMove:(id)sender;
- (IBAction)oHandlePaste:(id)sender;

@end
