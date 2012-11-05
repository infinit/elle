//
//  InfinitestData.m
//  Infinitest
//
//  Created by Simon Peccaud on 3/20/12.
//  Copyright 2012 infinit.io. All rights reserved.
//

#import "InfinitData.h"

static NSMutableArray *checkedFiles = nil;
static NSMutableArray *checkedData = nil;

@interface InfinitData () {}     // Class extension with private properties
    @property(strong, nonatomic) NSMutableArray                                             *files;
    @property(strong, nonatomic) NSFileManager                                              *fileMgr;
    @property(strong, nonatomic) id <UIDocumentInteractionControllerDelegate>                DICDelegate;
    @property(strong, nonatomic) NSEnumerator                                               *nextFile;
    @property(strong, nonatomic) UITableView                                                *_tableView;
@end

@implementation InfinitData

@synthesize files;
@synthesize fileMgr;
@synthesize filesDirectory;
@synthesize DICDelegate;
@synthesize currentlyRevealedCell;
@synthesize nextFile;
@synthesize _tableView;
@synthesize folderImage;
@synthesize plusImage;
@synthesize imageToSave;
@synthesize movieToSavePath;
@synthesize rootView;
@synthesize addViewButtonItem;
@synthesize checkView;
@synthesize uncheckedView;
@synthesize editMode;
@synthesize rootList;

- (id) init
{
    self = [super init];
    return self;
}

- (id) initWithDocumentInteractionControllerDelegate:(id <UIDocumentInteractionControllerDelegate>) dic filesDirectory:(NSString*) filesDir tableView:(UITableView*) tableView
{
    self = [super init];
    if (self) {
        DICDelegate = dic;

        files = [[NSMutableArray alloc] init];
        if (!checkedFiles)
            checkedFiles = [[NSMutableArray alloc] init];

        if (!checkedData)
            checkedData = [[NSMutableArray alloc] init];

        // Create file manager
        fileMgr = [NSFileManager defaultManager];

        // Point to Document directory
        filesDirectory = filesDir;

        _tableView = tableView;

        NSError*            error;
        NSArray*            array = [fileMgr contentsOfDirectoryAtPath:filesDirectory error:&error];
        nextFile = [array objectEnumerator];

        self.editMode = FALSE;
        self.avoidDoubleAction = FALSE;

        self->displayTextInputCell = NO;
        self->dirTextInput = NO;
        self->fileTextInput = NO;
        self.imageToSave = nil;
        self.folderImage = [UIImage imageNamed:@"folder.png"];
        self.plusImage = [UIImage imageNamed:@"plus.png"];
        self.checkView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"checked.png"]];
        self.checkView.frame = CGRectMake(280, 8, 30, 28);
        self.uncheckedView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"unchecked.png"]];
        self.uncheckedView.frame = CGRectMake(280, 8, 30, 28);

        [tableView setDataSource:self];
    }

    self.rootList = NO;

    return self;
}

- (BOOL) updateFromDirectory {
    NSString*             fileURL;
    NSInteger           i = 0;
    NSMutableDictionary*                networkNames = [[NSMutableDictionary alloc] init];

    [files removeAllObjects];

    if (self.rootList && token) {    // Online
        //NSLog(@"%@", [InfinitDevices singleton].devices);
        NSLog(@"<%@>, %@", [InfinitNetworks singleton].networks, token);

        InfinitNetwork  *ifNetwork;
        NSEnumerator *nextNetwork = [[InfinitNetworks singleton].networks objectEnumerator];

        while ((ifNetwork = nextNetwork.nextObject)) {
            [networkNames setObject:ifNetwork.name forKey:ifNetwork.name];
        }
    }

    if (self.rootList) {
        BOOL isDirectory = FALSE;
        while ((fileURL = [nextFile nextObject]) && (i++ < FILES_LIST_BATCH_SIZE))
            if ([[NSFileManager defaultManager] fileExistsAtPath:[self.filesDirectory stringByAppendingPathComponent:fileURL] isDirectory:&isDirectory]
                && isDirectory)
            {
                if (token && ![networkNames valueForKey:fileURL]) {  // Drive was deleted, erase local directory
                    [self.fileMgr removeItemAtPath:[self.filesDirectory stringByAppendingPathComponent:fileURL] error:nil];
                }
                else
                    [files addObject:[[InfinitFile alloc] initWithFileURL:[self.filesDirectory stringByAppendingPathComponent:fileURL] dicDelegate:DICDelegate]];
            }
    }
       /* while ((fileURL = [nextFile nextObject]) && (i++ < FILES_LIST_BATCH_SIZE)) {
            if ([fileURL isEqualToString:@"InfinitUInfo.plist"])
                i--;
            else
                [files addObject:[[InfinitFile alloc] initWithFileURL:[filesDirectory stringByAppendingPathComponent:fileURL] dicDelegate:DICDelegate]];
        }*/
    else
        while ((fileURL = [nextFile nextObject]) && (i++ < FILES_LIST_BATCH_SIZE))
            [files addObject:[[InfinitFile alloc] initWithFileURL:[filesDirectory stringByAppendingPathComponent:fileURL] dicDelegate:DICDelegate]];

    return (self.rootList || (fileURL != nil));
}

- (void) updateFromSearch:(NSString *) searchChars {
    NSArray *containsJ;

    if ([searchChars isEqualToString:@""])
    {
        [files removeAllObjects];
        [self resetLoadedFilesCounter];
        [self updateFromDirectory];
    }
    else {
        NSPredicate *bPredicate = [NSPredicate predicateWithFormat:@"SELF.dic.name contains[cd] %@", searchChars];
        containsJ = [files filteredArrayUsingPredicate:bPredicate];
        [files removeAllObjects];
        [files addObjectsFromArray:containsJ];
        [self._tableView reloadData];
    }
}

- (BOOL) insertFile:(NSString*) name atPosition:(int) row{
    NSString* path = [filesDirectory stringByAppendingPathComponent:name];

    [files insertObject:[[InfinitFile alloc] initWithFileURL:path dicDelegate:DICDelegate] atIndex:row];

    return TRUE;
}

- (BOOL) addDir:(NSString*) name {
    NSString* path = [filesDirectory stringByAppendingPathComponent:name];
    BOOL isDir = NO;
    if ([fileMgr fileExistsAtPath:path isDirectory:&isDir] && isDir)
        return FALSE;
    [fileMgr createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:nil];
    [files insertObject:[[InfinitFile alloc] initWithFileURL:[filesDirectory stringByAppendingPathComponent:name] dicDelegate:DICDelegate] atIndex:0];
    return TRUE;
}

- (void) addDirTextInputCell {
    self->dirTextInput = YES;

    [self toggleInputCell];
}

- (void) addFileTextInputCell {
    self->fileTextInput = YES;

    [self toggleInputCell];
}

- (void) addLinkTextInputCell {
    self->linkTextInput = YES;
    self->fileTextInput = YES;

    [self toggleInputCell];
}

- (void) toggleInputCell {
    BOOL set = !self->displayTextInputCell;

    if (set) {// Text input wasn't displayed
        [files insertObject:@"_" atIndex:0];
        [addViewButtonItem setEnabled:NO];
    }
    else {
        [addViewButtonItem setEnabled:YES   ];
    }

    self->displayTextInputCell = set;
   // self.lockReveal = set;
    gLockReveal = set;
    self._tableView.scrollEnabled = !set;
    [self._tableView setAllowsSelection:!set];
}

- (InfinitFile*) getValueAtIndex:(NSInteger) index {
    return ([files objectAtIndex:index]);
}

- (NSInteger) numberOfRowsInTableView:(UITableView *)tableView {
    if (self.rootList && token) // Online rootList
        return [[InfinitNetworks singleton].networks count];
    return [files count];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if (self.rootList && token) // Online rootList
        return [[InfinitNetworks singleton].networks count];
    return [files count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *SimpleTableIdentifier = @"FilesTableIdentifier";
    static NSString *DrivesTableIdentifier = @"DrivesTableIdentifier";

    NSUInteger row = [indexPath row];
    InfinitFile *ifFile;

    if (!self.rootList || !token)   // Isn't rootlist nor online
        ifFile = [self getValueAtIndex:row];

    if (self->displayTextInputCell && row == 0) {
        UITableViewCell* cell;
        NSString *placeHolderTxt;

        if (self->dirTextInput)
        {
            cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:@"dirTextInputCell"];
            cell.imageView.image = folderImage;
            placeHolderTxt = @"Name your folder (empty to cancel)";
        }
        else if (self->fileTextInput)
        {
            cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:@"fileTextInputCell"];
            cell.imageView.image = plusImage;
            placeHolderTxt = @"Name your file (empty to cancel)";
        }

        UITextField *txtField=[[UITextField alloc]initWithFrame:CGRectMake(cell.imageView.image.size.width * 1.4, 10, 310 - cell.imageView.image.size.width - 5, 34)];
        txtField.autoresizingMask=UIViewAutoresizingFlexibleHeight;
        txtField.autoresizesSubviews=YES;
        [txtField setBorderStyle:UITextBorderStyleNone];
        txtField.autocorrectionType = UITextAutocorrectionTypeNo;

        [txtField setPlaceholder:placeHolderTxt];

        [txtField setKeyboardType:UIKeyboardTypeNamePhonePad];
        [txtField setTextColor:[UIColor whiteColor]];
        txtField.returnKeyType = UIReturnKeyDone;

        [cell.contentView addSubview:txtField];
        [txtField setDelegate:(id) self];
        [txtField becomeFirstResponder];

        cell.contentView.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.8];

        return cell;
    }
    else
    {
        ZKRevealingTableViewCell *cell;
        //UITableViewCell*    driveCell;
        DriveCell*  driveCell;

        if (self.rootList)
        {
            self.folderImage = [UIImage imageNamed:@"drive-folder.png"];
            driveCell = [tableView dequeueReusableCellWithIdentifier:DrivesTableIdentifier];
        }
        else
            cell = [tableView dequeueReusableCellWithIdentifier:SimpleTableIdentifier];

        CGRect fileNameRect;

        if (self.rootList)
            fileNameRect = CGRectMake(65, 15, 270, 20);
        else
            fileNameRect = CGRectMake(45, 12, 270, 20);
        if (!self.rootList && cell == nil)
        {
            cell = [[ZKRevealingTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:SimpleTableIdentifier rootView:self.rootView];
            cell.delegate = self;

            cell.backView.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"DoubleShadowBack.png"]];

            cell.direction = ZKRevealingTableViewCellDirectionLeft;

            cell.deleteButton = [UIButton buttonWithType:UIButtonTypeCustom];

            cell.deleteButton.frame = CGRectMake(cell.backView.bounds.size.width - 100,
                                      cell.backView.bounds.origin.y,
                                      100,
                                      cell.backView.bounds.size.height - 1);

            [cell.deleteButton setImage:[UIImage imageNamed:@"delete.png"] forState:UIControlStateNormal];
            [cell.deleteButton addTarget:self action:@selector(deleteRow:) forControlEvents:UIControlEventTouchUpInside];

            cell.emailButton = [UIButton buttonWithType:UIButtonTypeCustom];

            cell.emailButton.frame = CGRectMake(cell.backView.bounds.size.width - 180,
                                      cell.backView.bounds.origin.y,
                                      100,
                                      cell.backView.bounds.size.height - 1);

            [cell.emailButton setImage:[UIImage imageNamed:@"mail.png"] forState:UIControlStateNormal];
            [cell.emailButton addTarget:self.rootView action:@selector(oPresentSendEmailFile:) forControlEvents:UIControlEventTouchUpInside];
            if (ifFile.isDirectory) {
                cell.emailButton.enabled = NO;
            }

            [cell.backView addSubview:cell.deleteButton];
            [cell.backView addSubview:cell.emailButton];

            if (ifFile.isDownloading) {
                [cell.contentView addSubview:ifFile.progressLabel];
                fileNameRect = CGRectMake(45, 12, 230, 20);
            }
            else if (self.editMode)
            {
                if (!ifFile.isDirectory) {
                    [ifFile.checkStatusView removeFromSuperview];
                    if (ifFile.isChecked) {
                        ifFile.checkStatusView = [[UIImageView alloc] initWithImage:checkView.image];
                        ifFile.checkStatusView.frame = checkView.frame;
                    }
                    else {
                        ifFile.checkStatusView = [[UIImageView alloc] initWithImage:uncheckedView.image];
                        ifFile.checkStatusView.frame = uncheckedView.frame;
                    }
                    [cell.contentView addSubview:ifFile.checkStatusView];
                    fileNameRect = CGRectMake(45, 12, 230, 20);
                }
            }

            [ifFile.fileNameLabel removeFromSuperview];
            ifFile.fileNameLabel.frame = fileNameRect;
            [cell.contentView addSubview:ifFile.fileNameLabel];

            if ([ifFile isDirectory])
                cell.imageView.image = self.folderImage;
            else
                cell.imageView.image = [ifFile.dic.icons objectAtIndex:1];
            cell.selectionStyle = UITableViewCellSelectionStyleNone;

            if (!cell.longTouchGesture) {
                cell.longTouchGesture = [[TaggedLongPressGestureRecognizer alloc] initWithTarget:self.rootView action:@selector(handleCellLongPress:)];
                cell.longTouchGesture.delegate = self.rootView;

                cell.longTouchGesture.fromFile = ifFile;

                [cell addGestureRecognizer:cell.longTouchGesture];
            }

            return cell;
        }
        else if (self.rootList) {  // Online Root List
            if (driveCell == nil) {
                driveCell = [[DriveCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:DrivesTableIdentifier];
                driveCell.imageView.image = self.folderImage;
                [driveCell.infoButton addTarget:self.rootView action:@selector(oShowDriveInfo:) forControlEvents:UIControlEventTouchUpInside];
                driveCell.contentView.backgroundColor = [[UIColor alloc] initWithPatternImage:[UIImage imageNamed:@"striped-background.png"]];
            }

            if (token) {    // Online
                NSArray*    networkKeys = [[InfinitNetworks singleton].networks allKeys];

                InfinitNetwork* network = [[InfinitNetworks singleton].networks objectForKey:[networkKeys objectAtIndex:row]];

                driveCell.textLabel.text = [network name];
                driveCell.infoButton.tag = row;
            }
            else {           // Offline
                driveCell.textLabel.text = ifFile.dic.name;
                [driveCell hideInfoButton];
            }

            return driveCell;
        }
        else //if (!self.rootList)
        {
            [[cell.contentView.subviews objectAtIndex:0] removeFromSuperview];
            if (cell.contentView.subviews.count)
                [[cell.contentView.subviews objectAtIndex:0] removeFromSuperview];

            if (ifFile.isDirectory) {
                cell.emailButton.enabled = NO;
                [cell.contentView addSubview:ifFile.checkStatusView];
            }

            if (ifFile.isDownloading) {
                [cell.contentView addSubview:ifFile.progressLabel];
                fileNameRect = CGRectMake(45, 12, 230, 20);
            }
            else if (self.editMode)
            {
                if (!ifFile.isDirectory) {
                    [ifFile.checkStatusView removeFromSuperview];
                    if (ifFile.isChecked) {
                        ifFile.checkStatusView = [[UIImageView alloc] initWithImage:checkView.image];
                        ifFile.checkStatusView.frame = checkView.frame;
                    }
                    else {
                        ifFile.checkStatusView = [[UIImageView alloc] initWithImage:uncheckedView.image];
                        ifFile.checkStatusView.frame = uncheckedView.frame;
                    }
                    [cell.contentView addSubview:ifFile.checkStatusView];
                    fileNameRect = CGRectMake(45, 12, 230, 20);
                }
            }

            [ifFile.fileNameLabel removeFromSuperview];
            ifFile.fileNameLabel.frame = fileNameRect;
            [cell.contentView addSubview:ifFile.fileNameLabel];

            if ([ifFile isDirectory]) {
                cell.imageView.image = [UIImage imageNamed:@"folder.png"];
            }
            else
                cell.imageView.image = [ifFile.dic.icons objectAtIndex:1];
            cell.selectionStyle = UITableViewCellSelectionStyleNone;

            if (!cell.longTouchGesture) {
                cell.longTouchGesture = [[TaggedLongPressGestureRecognizer alloc] initWithTarget:self.rootView action:@selector(handleCellLongPress:)];
                cell.longTouchGesture.delegate = self.rootView;

                [cell addGestureRecognizer:cell.longTouchGesture];
            }
            cell.longTouchGesture.fromFile = ifFile;
        }

        return cell;
    }
}

- (BOOL)cellShouldReveal:(ZKRevealingTableViewCell *)cell {
    if (gLockReveal)
        return NO;
    if (self.currentlyRevealedCell != cell)
    {
        [self.currentlyRevealedCell setRevealing:NO];
        self.currentlyRevealedCell = cell;
    }
    return YES;
}

- (void)cellDidBeginPan:(ZKRevealingTableViewCell *)cell {
    if (self.currentlyRevealedCell != cell)
    {
        [self.currentlyRevealedCell setRevealing:NO];
        self.currentlyRevealedCell = cell;
    }
}

- (void)cellDidReveal:(ZKRevealingTableViewCell *)cell {
    self.currentlyRevealedCell = cell;
}

- (BOOL)loadMoreFiles:(id)sender
{
    NSString*       fileURL;
    NSInteger       i = 0;

    while ((i++ < FILES_LIST_BATCH_SIZE) && (fileURL = [nextFile nextObject])) {
        [files addObject:[[InfinitFile alloc] initWithFileURL:[filesDirectory stringByAppendingPathComponent:fileURL] dicDelegate:DICDelegate]];
    }

    return (fileURL != nil);
}

- (void)resetLoadedFilesCounter {
    NSError*            error;
    NSArray*            array = [fileMgr contentsOfDirectoryAtPath:filesDirectory error:&error];
    nextFile = [array objectEnumerator];
}

- (IBAction)deleteRow:(id)sender {
    if (self.currentlyRevealedCell) {
        NSInteger row = [[self._tableView indexPathForCell:self.currentlyRevealedCell] row];
        self.currentlyRevealedCell = nil;

        [self.fileMgr removeItemAtPath:[self.filesDirectory stringByAppendingPathComponent:[[[self.files objectAtIndex:row] dic] name]] error:nil];

        [self.files removeObjectAtIndex:row];

        [self._tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:row inSection:0]] withRowAnimation:UITableViewRowAnimationFade];
    }
}

- (IBAction)deleteFile:(InfinitFile*) ifFile {
    NSLog(@"Before %i, remove %@", [files count], [ifFile dic].URL);

    InfinitFile*    nextIfFile;
    int      i = 0;
 //   NSEnumerator*   fileEnum = [self.files objectEnumerator];

    while (i < [self.files count]) {
        nextIfFile = [self.files objectAtIndex:i];
        if ([nextIfFile.dic.name isEqualToString:ifFile.dic.name])
           // [self.files removeObject:nextIfFile];
            [self.files removeObjectAtIndex:i];
        i++;
    }

    NSLog(@"After %i", [files count]);
}

-(void)textFieldDidEndEditing:(UITextField *)textField {
    [self._tableView beginUpdates];
    [self.files removeObjectAtIndex:0];
    [self._tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:0 inSection:0]] withRowAnimation:UITableViewRowAnimationFade];

    if (self->dirTextInput) {
        if (![textField.text isEqualToString:@""] && ![self.fileMgr fileExistsAtPath:textField.text] && [self addDir:[textField text]]) {
            [self._tableView insertRowsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:0 inSection:0]] withRowAnimation:UITableViewRowAnimationTop];
        }
        [self toggleInputCell];
        self->dirTextInput = NO;
    }
    else if (self->fileTextInput) {
        if (self.imageToSave) {
            if (![textField.text isEqualToString:@""] && ![self.fileMgr fileExistsAtPath:[filesDirectory stringByAppendingPathComponent:[textField.text stringByAppendingString:@".jpg"]]]) {
               // NSData* imageData = [NSData dataWithData:UIImagePNGRepresentation(self.imageToSave)];
                //[imageData writeToFile:[textField.text stringByAppendingString:@".png"] atomically:NO];

                [self saveData:UIImageJPEGRepresentation(self.imageToSave, 100) toFile:[textField.text stringByAppendingString:@".jpg"]];
                [self._tableView insertRowsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:0 inSection:0]] withRowAnimation:UITableViewRowAnimationTop];
            }
            imageToSave = nil;
        }
        else if (movieToSavePath) {
            if (![textField.text isEqualToString:@""] && ![self.fileMgr fileExistsAtPath:[filesDirectory stringByAppendingPathComponent:[textField.text stringByAppendingString:@".mov"]]]) {
                [self saveData:[NSData dataWithContentsOfURL:movieToSavePath] toFile:[[textField text] stringByAppendingString:@".mov"]];
                [self._tableView insertRowsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:0 inSection:0]] withRowAnimation:UITableViewRowAnimationTop];
            }
            movieToSavePath = nil;
        }
        else if (self->linkTextInput) {
            if (![textField.text isEqualToString:@""]) {
                [self._tableView endUpdates];

                // create the connection with the request
                // and start loading the data

                NSString *dlPAth = [[textField.text pathComponents] objectAtIndex:[[textField.text pathComponents] count] - 1];

                dlPAth = [filesDirectory stringByAppendingPathComponent:dlPAth];

                [self addFileDownloadFromFilePath:textField.text toFile:dlPAth];
            }
            self->linkTextInput = NO;

            [self toggleInputCell];
            self->fileTextInput = NO;

            return;
        }

        [self toggleInputCell];
        self->fileTextInput = NO;
    }
    [self._tableView endUpdates];
}

-(BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    return NO;
}

-(void) toggleLockReveal {
    if (gLockReveal) {
        gLockReveal = NO;
    }
    else {
        [self.currentlyRevealedCell setRevealing:NO];
        self.currentlyRevealedCell = nil;

        gLockReveal = YES;
    }
}

-(void) setLockReveal:(BOOL) set {
    gLockReveal = set;
}

-(void) saveData:(NSData*) data toFile:(NSString*) name {
    NSString* path = [self.filesDirectory stringByAppendingPathComponent:name];

    [self.fileMgr createDirectoryAtPath:self.filesDirectory withIntermediateDirectories:TRUE attributes:nil error:nil];

    [data writeToFile:path atomically:NO];
    [files insertObject:[[InfinitFile alloc] initWithFileURL:path dicDelegate:DICDelegate] atIndex:0];
    [self._tableView reloadData];
}

-(void) addFileDownloadFromFilePath: (NSString*) path toFile:(NSString*) filePath {
    [self.fileMgr createFileAtPath:filePath contents:nil attributes:nil];

    InfinitFile *dlFile = [[InfinitFile alloc] initWithFileURL:filePath dicDelegate:DICDelegate];

    [files insertObject:dlFile atIndex:0];

    [dlFile startDownloadWithFilePath:path tableView:self._tableView];

    [self._tableView reloadData];
}

-(void) insertFilePath:(NSString*) filePath {
    [files insertObject:[[InfinitFile alloc] initWithFileURL:filePath dicDelegate:DICDelegate] atIndex:0];
    [self._tableView insertRowsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:0 inSection:0]] withRowAnimation:UITableViewRowAnimationTop];
}

- (void) hideCheckMarks {
    InfinitFile *ifFile;
    NSEnumerator    *checkedEnumerator = [self.files objectEnumerator];

    while (ifFile = [checkedEnumerator nextObject]) {
        [ifFile.checkStatusView removeFromSuperview];
        ifFile.checkStatusView = nil;
        ifFile.isChecked = NO;
    }

    //self.lockReveal = FALSE;
    gLockReveal = NO;
}

- (void)selectionWillChange:(id <UITextInput>)textInput {};
- (void)selectionDidChange:(id <UITextInput>)textInput {};
- (void)textWillChange:(id <UITextInput>)textInput {};
- (void)textDidChange:(id <UITextInput>)textInput {};

- (NSMutableArray*) getCheckedFiles {
    return checkedFiles;
}

- (NSMutableArray*) getCheckedData {
    return checkedData;
}

@end
