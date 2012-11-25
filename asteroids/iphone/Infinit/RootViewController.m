//
//  RootViewController.m
//  Infinitest
//
//  Created by Simon Peccaud on 3/20/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import "RootViewController.h"
#import "InfinitAppDelegate.h"

static NSMutableArray *rootViewArray;
static UIToolbar  *editBar;
static CGRect editBarRevealedFrame;
static CGRect editBarHiddenFrame;

@interface RootViewController () {}
    @property (nonatomic, strong) UITapGestureRecognizer        *_tapGesture;
    @property (nonatomic, strong) UIPinchGestureRecognizer        *_pinchGesture;

    @property(strong, nonatomic) UITableView*   _rightSidebarView;
    @property(strong, nonatomic) RightSidebarDataSource*   _rightSidebarData;
@end

@implementation RootViewController

@synthesize oTableView;
@synthesize oSearchBar;
@synthesize data;
@synthesize loginViewController;
@synthesize _tapGesture;
@synthesize _rightSidebarView;
@synthesize _rightSidebarData;
@synthesize nextRootViewController;
@synthesize appView;
@synthesize tableViewEditModeFrame;
@synthesize tableViewInitialFrame;
@synthesize offlineCell;

- (void)viewDidLoad
{
    [super viewDidLoad];
    oSearchBar.autocorrectionType = UITextAutocorrectionTypeNo;
    [oSearchBar setShowsCancelButton:NO];
    self.navigationItem.revealSidebarDelegate = self;
    self.nextRootViewController = nil;

  //  if (!self.loginViewController || !self.data.rootList) {
    [[NSNotificationCenter defaultCenter]   addObserver:self
                                               selector:@selector(revealEditBar:)
                                                   name:@"didEnterEditMode"
                                                 object:nil];
    [[NSNotificationCenter defaultCenter]   addObserver:self
                                               selector:@selector(hideEditBar)
                                                   name:@"didExitEditMode"
                                                 object:nil];
  //  }
}

- (void) handleDidLogIn: (NSNotification *) notif {
    if (self.data == nil) {
        data = [[InfinitData alloc] initWithDocumentInteractionControllerDelegate:self filesDirectory:[NSHomeDirectory() stringByAppendingPathComponent:ROOT_FILE_DIR] tableView:self.tableView];

        self.data.folderImage = [UIImage imageNamed:@"drive-folder.png"];

        self.data.rootList = YES;
    }

    if (![data numberOfRowsInTableView:self.tableView] || (self.data.rootList && token)) {
        self.tableView.tableFooterView = nil;
        self->isFooterPresent = FALSE;
    }

    [self hideSearchBar];

    self.data.rootView = self;

    if (!editBar)
        [self setupEditBar];

    rootViewArray = [[NSMutableArray alloc] initWithObjects:self, nil];

    UITableViewCell * settingsCell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:nil];

    settingsCell.backgroundColor = [UIColor colorWithRed:0.4 green:0.7 blue:1 alpha:1];
    settingsCell.imageView.image = [UIImage imageNamed:@"settings.png"];
    settingsCell.textLabel.text = @"Settings";
    settingsCell.textLabel.backgroundColor = [UIColor clearColor];
    UITapGestureRecognizer *singleTapRecogniser = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oHandleSettingsTouch:)];
    [singleTapRecogniser setDelegate:self];
    singleTapRecogniser.numberOfTouchesRequired = 1;
    singleTapRecogniser.numberOfTapsRequired = 1;
    [settingsCell addGestureRecognizer:singleTapRecogniser];

    self.oTableView.tableHeaderView = settingsCell;

    [self.oTableView setRowHeight:55];      //self.oTableView.rowHeight + 0.2 * self.oTableView.rowHeight];
    self.oTableView.separatorColor = [UIColor blackColor];
    self.oTableView.separatorStyle = UITableViewCellSeparatorStyleNone;

    if (notif) { // Logged through the loginView form
        NSString *path = [NSHomeDirectory() stringByAppendingPathComponent:PLIST_PATH_COMPONENT];
        NSFileManager *fileManager = [NSFileManager defaultManager];

        if (![fileManager fileExistsAtPath: path])
        {
            NSError*    error;
            NSString *bundle = [[NSBundle mainBundle] pathForResource:@"InfinitUInfo" ofType:@"plist"];

            [fileManager copyItemAtPath:bundle toPath: path error:&error];
        }

        //path = [self.data.filesDirectory stringByAppendingPathComponent:@"InfinitUInfo.plist"];

        NSMutableDictionary *plistDico = [[NSMutableDictionary alloc] initWithContentsOfFile:path];

        [plistDico setObject:self.loginViewController.userMail forKey:@"rememberM"];
        [plistDico setObject:self.loginViewController.pswd forKey:@"rememberP"];

        if (self.loginViewController.oRememberSwitch.on)
            [plistDico setObject:@"YES" forKey:@"rememberMe?"];
        else if ([fileManager fileExistsAtPath: path])
            [plistDico setObject:@"NO" forKey:@"rememberMe?"];

        [plistDico writeToFile:path atomically:YES];

        self.presentedViewController.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
        [self.presentedViewController dismissModalViewControllerAnimated:YES];
    }
    else { // Logged from RootView (remembered user)
        [self viewWillAppear:TRUE];
    }
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
 //   [infinitAppDelegate putLoginBackView];

    [self.navigationController.navigationBar setBackgroundImage:[UIImage imageNamed:@"navigationbar-back.png"] forBarMetrics:UIBarMetricsDefault];

    if (!self.data)
    {
        [[NSNotificationCenter defaultCenter]   addObserver:self
                                                   selector:@selector(handleDidLogIn:)
                                                       name:@"didLogIn"
                                                     object:nil];
        [[NSNotificationCenter defaultCenter]   addObserver:self
                                                   selector:@selector(handleDidLogOut:)
                                                       name:@"didLogOut"
                                                     object:nil];

        self.loginViewController = [[LoginViewController alloc] initWithNibName:@"LoginViewController" bundle:NULL];

        NSString *path = [[NSHomeDirectory() stringByAppendingPathComponent:@"Documents"] stringByAppendingPathComponent:@"InfinitUInfo.plist"];
        NSFileManager *fileManager = [NSFileManager defaultManager];

        if ([fileManager fileExistsAtPath: path]) {
            NSMutableDictionary *savedDico= [[NSMutableDictionary alloc] initWithContentsOfFile: path];

            NSString* rememberM = [savedDico objectForKey:@"rememberM"];
            NSString* rememberP = [savedDico objectForKey:@"rememberP"];
            BOOL rememberMe = FALSE;

            if ([[savedDico objectForKey:@"rememberMe?"] isEqualToString:@"YES"])
                rememberMe = TRUE;

            if (!rememberMe) { // User not remembered, creentials asking
                [self.loginViewController.oRememberSwitch setOn:FALSE];
                [self pushLoginView:self];
            }
            else { // Remembered user, transparent login
                self.loginViewController.userMail = rememberM;
                self.loginViewController.pswd = rememberP;
                //[self.loginViewController oLogin:nil];

                NSDictionary *dictionary = [[NSDictionary alloc] initWithObjectsAndKeys:
                                            self.loginViewController.userMail,   @"email",
                                            self.loginViewController.pswd,   @"password",
                                            nil];

                if ([self.loginViewController loginRequest:dictionary]) {
                    [self handleDidLogIn:nil];
                    InfinitAppDelegate *infinitAppDelegate = [[UIApplication sharedApplication] delegate];
                    [infinitAppDelegate putRootBackView];
                }
            }
        }
        else
            [self pushLoginView:self];
    }
    else {
        if ([data numberOfRowsInTableView:self.tableView] == 0 && ![data updateFromDirectory]) {
            self.tableView.tableFooterView = nil;
            self->isFooterPresent = FALSE;
        }

        if (!self.data.rootList)
            [self setupNotifBarInset];

        [oTableView setDataSource:data];
        NSArray* pathComp = [data.filesDirectory pathComponents];
        self.title = [pathComp objectAtIndex:[pathComp count] - 1];
        if ([self.title isEqualToString:@"Infinit"]) {
            self.navigationItem.titleView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"logo-title.png"]];
        }

        if (self != [self.navigationController.viewControllers objectAtIndex:0]) { // Files list
            [self hideSearchBar];

            self._tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTap:)];
            self._tapGesture.delegate = self;

            self._pinchGesture = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinch:)];
            self._pinchGesture.delegate = self;
            [self.view addGestureRecognizer:self._pinchGesture];

            UIBarButtonItem *addButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(oToggleAddView:)];
            self.navigationItem.rightBarButtonItem = addButton;
            self.data.addViewButtonItem = self.navigationItem.rightBarButtonItem;
            if ([data numberOfRowsInTableView:self.tableView] != 0)
                self.oTableView.tableHeaderView = self.oSearchBar;
        }

        if (self.data.editMode && self.data.rootList)
            [[NSNotificationCenter defaultCenter] postNotificationName:@"didExitEditMode" object:self];

        self.tableView.backgroundColor = [UIColor clearColor];
        self.tableView.separatorColor = [UIColor clearColor];

        self.tableViewInitialFrame = self.tableView.frame;
        self.tableViewEditModeFrame = CGRectMake(self.tableViewInitialFrame.origin.x, self.tableViewInitialFrame.origin.y,
                                             self.tableViewInitialFrame.size.width, self.tableViewInitialFrame.size.height - 44);
        [self setupNotifBarInset];

        if (self.data.editMode)
        {
            CGRect editFrame = self.tableViewEditModeFrame;

            if (!token || [token isEqualToString:@""]) {
                editFrame.origin.y = 20;
                editFrame.size.height -= 20;
            }

            [self.tableView setFrame:editFrame];
        }
    }

    self.nextRootViewController = nil;

    [self.tableView reloadData];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (data)
    {
        if (self.data.rootList) {
            RootViewController *dirContentViewController;
            NSEnumerator    *rootViewEnumerator = [rootViewArray objectEnumerator];
            NSString        *filesDirectory;

            if (token) {
                NSArray*    networkKeys = [[InfinitNetworks singleton].networks allKeys];
                InfinitNetwork* network = [[InfinitNetworks singleton].networks objectForKey:[networkKeys objectAtIndex:[indexPath row]]];

                filesDirectory = [self.data.filesDirectory stringByAppendingPathComponent:network.name];
            }
            else {
                InfinitFile *ifFile = [self.data getValueAtIndex:[indexPath row]];

                filesDirectory = [self.data.filesDirectory stringByAppendingPathComponent:ifFile.dic.name];
            }

            while (dirContentViewController = [rootViewEnumerator nextObject]) {
                if ([dirContentViewController.data.filesDirectory isEqualToString:filesDirectory])
                    break;
            }

            if (!dirContentViewController) {
                dirContentViewController = [[RootViewController alloc] initWithNibName:@"RootViewController" bundle:nil];
                dirContentViewController.data = [[InfinitData alloc] initWithDocumentInteractionControllerDelegate:self filesDirectory:filesDirectory tableView:dirContentViewController.tableView];

                dirContentViewController.data.rootView = dirContentViewController;

                [rootViewArray addObject:dirContentViewController];
                // dirContentViewController.prevRootViewController = self;
            }

            self.nextRootViewController = dirContentViewController;
            [dirContentViewController.navigationItem.backBarButtonItem setEnabled:TRUE];
            [self.navigationController pushViewController:dirContentViewController animated:YES];
        }
        else
        {
            if (data.currentlyRevealedCell)
            {
                [data.currentlyRevealedCell setRevealing:NO];
                data.currentlyRevealedCell = nil;
            }
            else {
                if (self.oSearchBar && self.oSearchBar.isFirstResponder)
                    [self toggleSearchBarSelected];
                else {
                    InfinitFile* ifFile = [data getValueAtIndex:indexPath.row];

                    if (!ifFile.isDirectory) {
                        if (self.data.editMode) {
                            if (!ifFile.isDownloading) {
                                if (ifFile.isChecked) {
                                    [[self.data getCheckedData] removeObjectAtIndex:[[self.data getCheckedFiles] indexOfObject:ifFile]];
                                    [[self.data getCheckedFiles] removeObject:ifFile];
                                }
                                else {
                                    [[self.data getCheckedData] addObject:self.data];
                                    [[self.data getCheckedFiles] addObject:ifFile];
                                }

                                ifFile.isChecked = !ifFile.isChecked;
                                [ifFile.checkStatusView removeFromSuperview];

                                [self.tableView reloadRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationNone];
                                //[self hideSearchBar];
                            }
                        }
                        else if (![ifFile.dic presentPreviewAnimated:TRUE]) {}
                    }
                    else { // is a directory
                        // Check if next rootView already exists
                        RootViewController *dirContentViewController;
                        NSEnumerator    *rootViewEnumerator = [rootViewArray objectEnumerator];
                        NSString        *filesDirectory;

                        filesDirectory = [self.data.filesDirectory stringByAppendingPathComponent:ifFile.dic.name];

                        while (dirContentViewController = [rootViewEnumerator nextObject]) {
                            if ([dirContentViewController.data.filesDirectory isEqualToString:filesDirectory])
                                break;
                        }

                        if (!dirContentViewController) {
                            dirContentViewController = [[RootViewController alloc] initWithNibName:@"RootViewController" bundle:nil];
                            dirContentViewController.data = [[InfinitData alloc] initWithDocumentInteractionControllerDelegate:self filesDirectory:filesDirectory tableView:dirContentViewController.tableView];

                            dirContentViewController.data.rootView = dirContentViewController;

                            [rootViewArray addObject:dirContentViewController];
                           // dirContentViewController.prevRootViewController = self;
                        }

                        self.nextRootViewController = dirContentViewController;

                        //self.data.getEditMode = tmpEditMode;

                        if (self.data.editMode)
                        {
                            //[dirContentViewController.tableView setFrame:dirContentViewController.tableViewEditModeFrame];
                            dirContentViewController.data.editMode = TRUE;
                            //[dirContentViewController revealEditBar];

                            // dirContentViewController.data.lockReveal = TRUE;
                            [dirContentViewController.data setLockReveal:TRUE];
                            //  dirContentViewController.editBar = self.editBar;
                        }
                        [dirContentViewController.navigationItem.backBarButtonItem setEnabled:TRUE];
                        [self.navigationController pushViewController:dirContentViewController animated:YES];
                    }
                }
            }
        }
    }
}

- (void)searchBarTextDidBeginEditing:(UISearchBar *)searchBar {
    self.oSearchBar = searchBar;

    [self toggleSearchBarSelected];
}

- (void)searchBarCancelButtonClicked:(UISearchBar *)searchBar {
    searchBar.text=@"";

    [searchBar setShowsCancelButton:NO animated:YES];
    [searchBar resignFirstResponder];

    [self toggleSearchBarSelected];

    [self.data updateFromSearch:searchBar.text];
    [self.oTableView reloadData];
}

- (void)searchBarSearchButtonClicked:(UISearchBar *)searchBar {
    [searchBar setShowsCancelButton:NO animated:YES];
    [searchBar resignFirstResponder];

    [self.data updateFromSearch:searchBar.text];
    [self.oTableView reloadData];

    [self toggleSearchBarSelected];
}

- (void)toggleSearchBarSelected {
    BOOL set = !self.oSearchBar.isFirstResponder;

    [self.oSearchBar setShowsCancelButton:!set animated:YES];

    self.oTableView.scrollEnabled = set;
    [self.navigationItem.rightBarButtonItem setEnabled:set];
    [self.data toggleLockReveal];

    if (set) // Bar was selected
        [self.oSearchBar resignFirstResponder];
    else
        [self alignSearchBar];
}

- (IBAction) pushLoginView: (id) sender {
    [self presentModalViewController:loginViewController animated:NO];
}

- (UIViewController *)documentInteractionControllerViewControllerForPreview:(UIDocumentInteractionController *)controller {
    return self;
}

- (void) hideSearchBar {
    return;     // Disabled

    if (self.oSearchBar && self.oSearchBar == self.tableView.tableHeaderView) {
        CGRect searchBarFrame = self.oSearchBar.frame;
        CGFloat searchBarHeight = searchBarFrame.size.height;
        CGPoint offsetFrame = CGPointMake(self.oTableView.contentOffset.x,searchBarHeight + self.oTableView.contentOffset.y);

        [self.oTableView setContentOffset:offsetFrame];
    }
    // [self.oTableView reloadData];
}

- (void) alignSearchBar {
    CGPoint offsetFrame = CGPointMake(self.oTableView.contentOffset.x,0);

    [self.oTableView setContentOffset:offsetFrame];
   // [self.tableView reloadData];
}

- (void) handleDidLogOut: (NSNotification *) notif {
    [self.loginViewController showDeleteLocalFilesWarning:FALSE];
    [self.loginViewController showSpinner:FALSE];
    [self.loginViewController showNetworkError:FALSE];
    [self.loginViewController showAuthError:FALSE offline:FALSE];
    [self.loginViewController.oPassField setEnabled:FALSE];
    [self.navigationController popViewControllerAnimated:NO];

    [self presentModalViewController:self.loginViewController animated:NO];
    [[super tabBarController] setSelectedIndex:0];

    NSString *path = [[NSHomeDirectory() stringByAppendingPathComponent:@"Documents"] stringByAppendingPathComponent:@"InfinitUInfo.plist"];
    NSFileManager *fileManager = [NSFileManager defaultManager];

    if (![fileManager fileExistsAtPath: path])
    {
        NSError*    error;
        NSString *bundle = [[NSBundle mainBundle] pathForResource:@"InfinitUInfo" ofType:@"plist"];

        [fileManager copyItemAtPath:bundle toPath: path error:&error];
        NSLog(@">> WROTE: %@", error);
    }

    NSMutableDictionary *plistDico = [[NSMutableDictionary alloc] initWithContentsOfFile: path];

    self.loginViewController.pswd = @"";
    self.loginViewController.userMail = @"";

    [plistDico setObject:@"NO" forKey:@"rememberMe?"];

    [plistDico writeToFile:path atomically:YES];

    data = nil;
    [[InfinitNetworks singleton].networks removeAllObjects];
    [InfinitNetworks destroySingleton];
    [rootViewArray removeAllObjects];

    [self.loginViewController logoutRequest];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];

    if (self.data.editMode)
        [self.tableView setFrame:self.tableViewEditModeFrame];

    if (self.data.rootList)
        [self setupNotifBarInset];
}

- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];

    [self.data resetLoadedFilesCounter];
}

- (void)viewDidDisappear:(BOOL)animated
{
	[super viewDidDisappear:animated];
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if (data)
        return [data numberOfRowsInTableView:self.tableView];
    return 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }

    if (data) {
        NSUInteger row = [indexPath row];
        cell.textLabel.text = [data getValueAtIndex:row].dic.name;
    }

    return cell;
}

-(void)tableView:(UITableView*)tableView willBeginEditingRowAtIndexPath:(NSIndexPath *)indexPath {
}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath {
    return UITableViewCellEditingStyleDelete;
}

/*- (IBAction)handleSwipeLeft:(UIGestureRecognizer *)sender {
    if (self.navigationItem.rightBarButtonItem.style != UIBarButtonItemStyleDone)
        [self.navigationController popViewControllerAnimated:YES];
}*/

- (IBAction)handleTap:(UIGestureRecognizer *)sender {
    if (self.navigationItem.rightBarButtonItem.style == UIBarButtonItemStyleDone)
        [self oToggleAddView:self];
}

- (IBAction)handlePinch:(UIPinchGestureRecognizer *)sender {
    // Pinch in

    if (![self.data numberOfRowsInTableView:self.tableView] || (self.navigationItem.rightBarButtonItem.style == UIBarButtonItemStyleDone))
        return;

    if (sender.velocity < 0) {
        if (self.data.editMode || sender.velocity > -0.95)
            return;
      //  [self revealEditBar];
        [[NSNotificationCenter defaultCenter] postNotificationName:@"didEnterEditMode" object:self];
    }
    else // Pinch out
    {
        if (!self.data.editMode || sender.velocity < 0.95)
            return;

        [[NSNotificationCenter defaultCenter] postNotificationName:@"didExitEditMode" object:self];
        //[self hideEditBar];
    }
}

- (IBAction)handleCellLongPress:(TaggedLongPressGestureRecognizer *)sender {
    /*if (self.data.avoidDoubleAction) {
        self.data.avoidDoubleAction = FALSE;
        return;
    }

    self.data.avoidDoubleAction = TRUE;
    */

    if (sender.state != UIGestureRecognizerStateBegan)
        return;

    if (self.data.editMode)
        [[NSNotificationCenter defaultCenter] postNotificationName:@"didExitEditMode" object:self];
    else {
        [[NSNotificationCenter defaultCenter] postNotificationName:@"didEnterEditMode" object:self];
        //[self revealEditBar];

        InfinitFile *ifFile = sender.fromFile;

        if (!ifFile.isDirectory && !ifFile.isDownloading) {
            [[self.data getCheckedFiles] addObject:ifFile];
            [[self.data getCheckedData] addObject:self.data];

            ifFile.isChecked = !ifFile.isChecked;
            [ifFile.checkStatusView removeFromSuperview];

            [self.tableView reloadData];

         //   [self hideSearchBar];
        }
    }
}

- (void) revealEditBar: (NSNotification*) notification {
    if (!self.data.editMode)
    {
        self.data.editMode = TRUE;

        if (![[notification object] isEqual:self])
            return;

        UIView* keyWindow = [[UIApplication sharedApplication] keyWindow];

        [[self.data getCheckedFiles] removeAllObjects];
        [[self.data getCheckedData] removeAllObjects];

        editBarRevealedFrame = CGRectMake(keyWindow.frame.origin.x,
                                          keyWindow.frame.size.height - 44,
                                          keyWindow.frame.size.width,
                                          44);

        editBarHiddenFrame = CGRectMake(keyWindow.frame.origin.x,
                                        keyWindow.frame.size.height,
                                        keyWindow.frame.size.width,
                                        44);

        editBar.frame = editBarHiddenFrame;
        CGRect editFrame = self.tableViewEditModeFrame;

        if (!token || [token isEqualToString:@""]) {
            editFrame.origin.y = 20;
            editFrame.size.height -= 20;
        }

        [UIView animateWithDuration:0.25 animations:^{
            [self.tableView setFrame:editFrame];

            //if (editBar.frame.origin.y != editBarRevealedFrame.origin.y)
            //{

            editBar.frame = editBarRevealedFrame;
            editBar.backgroundColor = [UIColor greenColor];
            if ([keyWindow.subviews indexOfObject:editBar] == NSNotFound) {
                [keyWindow addSubview:editBar];
            }
            [keyWindow bringSubviewToFront:editBar];
            //}
        }];
       // [self.data setLockReveal:TRUE];
        [self.tableView reloadData];
    }
}

- (void) hideEditBar {
    if (self.data.editMode)
    {
        UIView* keyWindow = [[UIApplication sharedApplication] keyWindow];

        editBarHiddenFrame = CGRectMake(keyWindow.frame.origin.x,
                                        keyWindow.frame.size.height,
                                        keyWindow.frame.size.width,
                                        44);

        [self.data setEditMode:FALSE];
        [UIView animateWithDuration:0.25 animations:^{
            // Repostion tableView frame while hiding search bar
            // [self hideSearchBar]


            [self.tableView setFrame:self.tableViewInitialFrame];
            [self hideSearchBar];
            if (editBar.frame.origin.y != editBarHiddenFrame.origin.y)
            {
                editBar.frame = editBarHiddenFrame;
            }
            [self.data.checkView removeFromSuperview];
            [self.data.uncheckedView removeFromSuperview];
        }];

        //  [self hideSearchBar];
        [self.tableView reloadData];
        //[self.data toggleLockReveal];

        [self.data hideCheckMarks];
    }
}

- (void) setupEditBar {
    editBar = [[UIToolbar alloc] init];

    UIView* keyWindow = [[UIApplication sharedApplication] keyWindow];

    editBarRevealedFrame = CGRectMake(keyWindow.frame.origin.x,
                                      keyWindow.frame.size.height - 44,
                                      keyWindow.frame.size.width,
                                      44);
    editBarHiddenFrame = CGRectMake(editBarRevealedFrame.origin.x,
                                    editBarRevealedFrame.origin.y,
                                    editBarRevealedFrame.size.width,
                                    editBarRevealedFrame.size.height);

    editBar.frame = editBarHiddenFrame;

    [editBar insertSubview:[[UIImageView alloc] initWithImage:[UIImage imageNamed:@"navigationbar-back.png"]] atIndex:1];

    UIBarButtonItem *flexibleSpace = [[UIBarButtonItem alloc]
                                      initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace
                                      target:nil action:nil];

    UIBarButtonItem *pinButton = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"pin-up.png"] style:UIBarButtonItemStylePlain
                                                                 target:self action:@selector(oHandlePin:)];

    UILabel *spaceLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 20, 20)];
    [spaceLabel setTextColor:[UIColor clearColor]];
    [spaceLabel setBackgroundColor:[UIColor clearColor]];
    UIBarButtonItem *spaceBarLabel = [[UIBarButtonItem alloc] initWithCustomView:spaceLabel];

    UIBarButtonItem *moveButton = [[UIBarButtonItem alloc] initWithTitle:@"Move" style:UIBarButtonItemStyleBordered
                                                                 target:self action:@selector(oHandleMove:)];
    UILabel *orLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 17, 20)];
    [orLabel setText:@"or"];
    [orLabel setTextColor:[UIColor whiteColor]];
    [orLabel setBackgroundColor:[UIColor clearColor]];
    UIBarButtonItem *orBarLabel = [[UIBarButtonItem alloc] initWithCustomView:orLabel];

    UIBarButtonItem *pasteButton = [[UIBarButtonItem alloc] initWithTitle:@"Paste" style:UIBarButtonItemStyleBordered
                                                                   target:self action:@selector(oHandlePaste:)];

    UILabel *hereLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 35, 20)];
    [hereLabel setText:@"here"];
    [hereLabel setTextColor:[UIColor whiteColor]];
    [hereLabel setBackgroundColor:[UIColor clearColor]];
    UIBarButtonItem *hereBarLabel = [[UIBarButtonItem alloc] initWithCustomView:hereLabel];

    [editBar setItems:[[NSArray alloc] initWithObjects:flexibleSpace, pinButton,
                            spaceBarLabel, flexibleSpace,
                            flexibleSpace, moveButton,
                            flexibleSpace, orBarLabel,
                            flexibleSpace, pasteButton,
                            flexibleSpace, hereBarLabel,
                            flexibleSpace, nil]];
   // [self.appView addSubview:editBar];
}

- (void)refreshHeader {
    [self performSelector:@selector(stopHeaderLoading) withObject:nil afterDelay:0.5];

    NSDictionary *dictionary = [[NSDictionary alloc] initWithObjectsAndKeys:
                                self.loginViewController.userMail,   @"email",
                                self.loginViewController.pswd,   @"password",
                                nil];
    [self.loginViewController loginRequest:dictionary];
}

- (void)refreshFooter {
    if (![data loadMoreFiles:self] && token) {
        self.tableView.tableFooterView = nil;
        self->isFooterPresent = FALSE;
    }
    [self.tableView reloadData];
    [self performSelector:@selector(stopFooterLoading) withObject:nil afterDelay:1.0];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (IBAction)oPresentFolderTextInput:(id)sender {
    [self oToggleAddView:self];
    if ([self.oTableView numberOfRowsInSection:0] > 2) {
        [self.oTableView scrollToRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0] atScrollPosition:UITableViewScrollPositionTop animated:YES];
    }
    [self.data addDirTextInputCell];
    [self.tableView insertRowsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:0 inSection:0]] withRowAnimation:UITableViewRowAnimationTop];
}

- (IBAction)oToggleAddView:(id)sender {
    if (self.navigationItem.rightBarButtonItem.style != UIBarButtonItemStyleDone) { // Isn't currently revealed
        self.navigationItem.rightBarButtonItem.style = UIBarButtonItemStyleDone;
        [self.view addGestureRecognizer:self._tapGesture];
        [[NSNotificationCenter defaultCenter] postNotificationName:@"didExitEditMode" object:self];
        //[self hideEditBar];
        [self.navigationItem setHidesBackButton:YES animated:YES];
    }
    else {  // Is currently revealed
        self.navigationItem.rightBarButtonItem.style = UIBarButtonItemStylePlain;
        [self.view removeGestureRecognizer:self._tapGesture];
        [self.navigationItem setHidesBackButton:NO animated:YES];
    }
    [self.data toggleLockReveal];
    [self.navigationController toggleRevealState:JTRevealedStateRight];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
}

- (UIView *)viewForRightSidebar {
    CGRect viewFrame = self.navigationController.applicationViewFrame;
    UITableView *view = self._rightSidebarView;
    if (!view) {
        view = self._rightSidebarView = [[UITableView alloc] initWithFrame:CGRectZero];

        view.delegate   = self;
        view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"rightsidebar-back.png"]];//[UIColor viewFlipsideBackgroundColor];
        view.separatorStyle = UITableViewCellSeparatorStyleNone;
        _rightSidebarData = [[RightSidebarDataSource alloc] initWithTableViewController:self];
        view.dataSource = _rightSidebarData;
    }
    view.frame = CGRectMake(self.navigationController.view.frame.size.width - 80, viewFrame.origin.y + self.navigationController.navigationBar.bounds.size.height - 1, 80, viewFrame.size.height);
    view.rowHeight = 80;
    [view setBounces:NO];
    [view setDirectionalLockEnabled:YES];
    [view setScrollEnabled:NO];

    return view;
}

-(IBAction)oPresentSingleShotView:(id)sender {
    [self oToggleAddView:self];
    [self.navigationItem.rightBarButtonItem setEnabled:NO];
    [self startCameraControllerFromViewController:self usingDelegate:self];
}

- (BOOL) startCameraControllerFromViewController: (UIViewController*) controller
                                   usingDelegate: (id <UIImagePickerControllerDelegate,
                                                   UINavigationControllerDelegate>) delegate {

    if (([UIImagePickerController isSourceTypeAvailable:
          UIImagePickerControllerSourceTypeCamera] == NO)
        || (delegate == nil)
        || (controller == nil))
        return NO;

    UIImagePickerController *cameraUI = [[UIImagePickerController alloc] init];
    cameraUI.sourceType = UIImagePickerControllerSourceTypeCamera;

    cameraUI.mediaTypes =
    [UIImagePickerController availableMediaTypesForSourceType:
     UIImagePickerControllerSourceTypeCamera];

    cameraUI.allowsEditing = NO;
    cameraUI.delegate = delegate;

    [controller presentModalViewController: cameraUI animated: YES];
    return YES;
}

- (void) imagePickerControllerDidCancel: (UIImagePickerController *) picker {
    [self.navigationController dismissModalViewControllerAnimated:YES];
}

- (void) imagePickerController: (UIImagePickerController *) picker
 didFinishPickingMediaWithInfo: (NSDictionary *) info {

    UIImage *originalImage, *editedImage;

    if ([[info objectForKey:UIImagePickerControllerMediaType] isEqualToString:@"public.image"]) {

        editedImage = (UIImage *) [info objectForKey:
                                   UIImagePickerControllerEditedImage];
        originalImage = (UIImage *) [info objectForKey:
                                     UIImagePickerControllerOriginalImage];

        if (editedImage) {
            self.data.imageToSave = editedImage;
        } else {
            self.data.imageToSave = originalImage;
        }

        [self.data addFileTextInputCell];
    }

    if ([[info objectForKey:UIImagePickerControllerMediaType] isEqualToString:@"public.movie"]) {
        self.data.movieToSavePath = [info objectForKey:UIImagePickerControllerMediaURL];

        [self.data addFileTextInputCell];
    }

    [self.navigationController dismissModalViewControllerAnimated:YES];
}

-(IBAction)oPresentPhotoLibrary:(id)sender
{
    if ([UIImagePickerController isSourceTypeAvailable:
        UIImagePickerControllerSourceTypePhotoLibrary])
    {
        [self oToggleAddView:self];
        [self.navigationItem.rightBarButtonItem setEnabled:NO];
        UIImagePickerController *picker= [[UIImagePickerController alloc]init];
        picker.delegate = self;
        picker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
        [self presentModalViewController:picker animated:YES];
    }
}

-(IBAction)oPresentSendEmailFile:(id)sender {
    if ([MFMailComposeViewController canSendMail] && self.data.currentlyRevealedCell) {
        NSInteger row = [[self.tableView indexPathForCell:self.data.currentlyRevealedCell] row];
        InfinitFile* file = [self.data getValueAtIndex:row];
        NSData* fileData = [NSData dataWithContentsOfFile:[self.data.filesDirectory stringByAppendingPathComponent:[[file dic] name]]];

        MFMailComposeViewController *mailController = [[MFMailComposeViewController alloc] init];
        [mailController setSubject:nil];
        [mailController setMessageBody:nil isHTML:NO];
        [mailController addAttachmentData:fileData mimeType:nil fileName:[file.dic name]];
        [mailController setMailComposeDelegate:self];
        [self presentModalViewController:mailController animated:YES];
    }
}

- (void)mailComposeController:(MFMailComposeViewController*)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error
{
    [self dismissModalViewControllerAnimated:YES];

    [self.data.currentlyRevealedCell setRevealing:NO];
    self.data.currentlyRevealedCell = nil;
}

-(IBAction)oPresentLinkTextInput:(id)sender {
    [self oToggleAddView:self];
    if ([self.oTableView numberOfRowsInSection:0] > 2) {
       [self.oTableView scrollToRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0] atScrollPosition:UITableViewScrollPositionTop animated:YES];
    }
    [self.navigationItem.rightBarButtonItem setEnabled:NO];
    [self.data addLinkTextInputCell];
    [self.tableView insertRowsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:0 inSection:0]] withRowAnimation:UITableViewRowAnimationTop];
}

- (void)handleDocumentOpenURL:(NSURL *)url {
    NSString    *name = [[url pathComponents] objectAtIndex:[[url pathComponents] count] - 1];
    NSString    *inboxPath = [[NSHomeDirectory() stringByAppendingPathComponent:ROOT_FILE_DIR] stringByAppendingPathComponent:@"Inbox"];
    RootViewController  *presentedRoot = [self nextRootViewController];

    if (!presentedRoot)
        presentedRoot = self;
    else if (presentedRoot.nextRootViewController)
        while (presentedRoot.nextRootViewController) {
            presentedRoot = presentedRoot.nextRootViewController;
        }

    [[NSFileManager defaultManager] moveItemAtPath:[inboxPath stringByAppendingPathComponent:name] toPath:[presentedRoot.data.filesDirectory stringByAppendingPathComponent:name] error:nil];

    if ([[[NSFileManager defaultManager] contentsOfDirectoryAtPath:inboxPath error:nil] count] == 0)
        [[NSFileManager defaultManager] removeItemAtPath:inboxPath error:nil];

    [presentedRoot.data insertFilePath:[presentedRoot.data.filesDirectory stringByAppendingPathComponent:name]];
}

- (IBAction)oHandleMove:(id)sender {
    NSEnumerator    *checkedEnumerator = [[self.data getCheckedFiles] objectEnumerator];
    InfinitFile     *ifFile;

    RootViewController  *presentedRoot = [self nextRootViewController];
    NSString    *newFilePath = [presentedRoot.data.filesDirectory stringByAppendingPathComponent:ifFile.dic.name];
    int         arrayCount = 0;

    BOOL isDir;
    int row = 0;

    if (!presentedRoot)
        presentedRoot = self;
    else if (presentedRoot.nextRootViewController)
        while (presentedRoot.nextRootViewController) {
            presentedRoot = presentedRoot.nextRootViewController;
        }

    [[NSNotificationCenter defaultCenter] postNotificationName:@"didExitEditMode" object:self];

    while (ifFile = [checkedEnumerator nextObject]) {
        newFilePath = [presentedRoot.data.filesDirectory stringByAppendingPathComponent:ifFile.dic.name];

        if ([[NSFileManager defaultManager] fileExistsAtPath:newFilePath isDirectory:&isDir])
            continue;

        if ([[NSFileManager defaultManager] moveItemAtPath:ifFile.dic.URL.path toPath:newFilePath error:nil]) {

            [presentedRoot.tableView beginUpdates];
            [presentedRoot.data insertFile:ifFile.dic.name atPosition:row];
            [presentedRoot.tableView insertRowsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:row++ inSection:0]] withRowAnimation:UITableViewRowAnimationAutomatic];
            [presentedRoot.tableView endUpdates];

            InfinitData*    ifData = [[self.data getCheckedData] objectAtIndex:arrayCount];

            [ifData deleteFile:ifFile];
           // [ifData resetLoadedFilesCounter];

//            RootViewController*     fileRootView = ifData.rootView;
//            [fileRootView.tableView reloadData];
        }
        arrayCount++;
    }

    NSEnumerator*   checkedDataEnumerator = [[self.data getCheckedData] objectEnumerator];
    InfinitData*    nextData;

    while (nextData = [checkedDataEnumerator nextObject]) {
        [nextData resetLoadedFilesCounter];
    }

    //[[self.data getCheckedData] removeAllObjects];
    //[[self.data getCheckedFiles] removeAllObjects];
}

- (IBAction)oHandlePaste:(id)sender {
    NSEnumerator    *checkedEnumerator = [[self.data getCheckedFiles] objectEnumerator];
    InfinitFile     *ifFile;

    RootViewController  *presentedRoot = [self nextRootViewController];
    NSString    *newFilePath = [presentedRoot.data.filesDirectory stringByAppendingPathComponent:ifFile.dic.name];
    BOOL isDir;
    int row = 0;

    if (!presentedRoot)
        presentedRoot = self;
    else if (presentedRoot.nextRootViewController)
        while (presentedRoot.nextRootViewController) {
            presentedRoot = presentedRoot.nextRootViewController;
    }

    [[NSNotificationCenter defaultCenter] postNotificationName:@"didExitEditMode" object:self];

    while (ifFile = [checkedEnumerator nextObject]) {
        newFilePath = [presentedRoot.data.filesDirectory stringByAppendingPathComponent:ifFile.dic.name];

        if ([[NSFileManager defaultManager] fileExistsAtPath:newFilePath isDirectory:&isDir])
            continue;

        [[NSFileManager defaultManager] copyItemAtPath:ifFile.dic.URL.path toPath:newFilePath error:nil];

        [presentedRoot.tableView beginUpdates];
        [presentedRoot.data insertFile:ifFile.dic.name atPosition:row];
        [presentedRoot.tableView insertRowsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:row++ inSection:0]] withRowAnimation:UITableViewRowAnimationAutomatic];
        [presentedRoot.tableView endUpdates];
    }
}

- (IBAction)oHandleSettingsTouch:(id)sender {
    SettingsViewController  *settingsViewController = [[SettingsViewController alloc] initWithNibName:@"SettingsViewController" bundle:NULL];

    [settingsViewController.navigationItem.backBarButtonItem setEnabled:TRUE];
    [settingsViewController setTitle:@"Settings"];
    [self.navigationController pushViewController:settingsViewController animated:YES];
}

- (IBAction)oShowDriveInfo:(id)sender {
    UIButton* driveInfoButton = sender;
    NSArray*    networkKeys = [[InfinitNetworks singleton].networks allKeys];

    InfinitNetwork* network = [[InfinitNetworks singleton].networks objectForKey:[networkKeys objectAtIndex:driveInfoButton.tag]];

    DriveInfoViewController *driveInfoViewController = [[DriveInfoViewController alloc] initWithInfinitNetwork:network];

    [self.navigationController pushViewController:driveInfoViewController animated:YES];

    //    [self presentModalViewController:driveInfoViewController animated:FALSE];
}

- (void)setupNotifBarInset {
    if (!token || [token isEqualToString:@""]) {    // Offline
        [self.tableView setContentOffset:CGPointMake(0, -20)];
        [[NSNotificationCenter defaultCenter] postNotificationName:@"revealNotifBar" object:@"Network unreachable"];
    }
    else {
        [self.tableView setContentOffset:CGPointMake(0, 0)];
        [[NSNotificationCenter defaultCenter] postNotificationName:@"hideNotifBar" object:nil];
    }
}

@end