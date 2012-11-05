//
//  InfinitestData.m
//  Infinitest
//
//  Created by Simon Peccaud on 3/20/12.
//  Copyright 2012 infinit.io. All rights reserved.
//

#import "InfinitestData.h"

@interface InfinitestData () {}     // Class extension with private properties
    @property(strong, nonatomic) NSMutableArray                                             *files;
    @property(strong, nonatomic) NSFileManager                                              *fileMgr;
    @property(strong, nonatomic) id <UIDocumentInteractionControllerDelegate>                DICDelegate;
    @property(strong, nonatomic) NSEnumerator                                               *nextFile;
    @property(strong, nonatomic) UITableView                                                *_tableView;
@end

@implementation InfinitestData

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
        
        // Create file manager
        fileMgr = [NSFileManager defaultManager];
        
        // Point to Document directory
        filesDirectory = filesDir;
        
        _tableView = tableView;
        
        NSError*            error;
        NSArray*            array = [fileMgr contentsOfDirectoryAtPath:filesDirectory error:&error];
        nextFile = [array objectEnumerator];

        self->displayTextInputCell = NO;
        self->dirTextInput = NO;
        self->fileTextInput = NO;
        self->lockReveal = NO;
        self.imageToSave = nil;
        self.folderImage = [UIImage imageNamed:@"folder.png"];
        self.plusImage = [UIImage imageNamed:@"plus.png"];
    }

    return self;
}

- (BOOL) updateFromDirectory {
    //NSError*            error;
    //NSArray*            array = [fileMgr contentsOfDirectoryAtPath:filesDirectory error:&error];
    //NSEnumerator*       e = [array objectEnumerator];
    NSString*             fileURL;
    NSInteger           i = 0;
    
    [files removeAllObjects];
    while ((fileURL = [nextFile nextObject]) && (i++ < FILES_LIST_BATCH_SIZE)) {
        [files addObject:[[InfinitestFile alloc] initWithFileURL:[filesDirectory stringByAppendingPathComponent:fileURL] dicDelegate:DICDelegate]];   
    }
    
    return (fileURL != nil);
}

- (void) updateFromSearch:(NSString *) searchChars {
//    NSEnumerator*       e = [files objectEnumerator];
//    InfinitestFile*     file;

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

- (void) addFile {
    // File we want to create in the documents directory
    NSString *filePath = [[filesDirectory 
                          stringByAppendingPathComponent:@"file"] stringByAppendingFormat:@"%d.txt", [files count]];
    // String to write
    NSString *str = [@"Content" stringByAppendingFormat:@"%d", [files count]];

    // For error information
    NSError *error;
    
    // Write the file
    [str writeToFile:filePath atomically:YES encoding:NSUTF8StringEncoding error:&error];
    
    [files addObject:[@"file" stringByAppendingFormat:@"%d.txt", [files count]]];
}

- (BOOL) addDir:(NSString*) name {
    NSString* path = [filesDirectory stringByAppendingPathComponent:name];
    BOOL isDir = NO;
    if ([fileMgr fileExistsAtPath:path isDirectory:&isDir] && isDir)
        return FALSE;
    [fileMgr createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:nil];
    [files insertObject:[[InfinitestFile alloc] initWithFileURL:[filesDirectory stringByAppendingPathComponent:name] dicDelegate:DICDelegate] atIndex:0];
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

- (void) toggleInputCell {
    BOOL set = !self->displayTextInputCell;

    if (set) // Text input wasn't displayed
        [files insertObject:@"_" atIndex:0];
    
    self->displayTextInputCell = set;
    self->lockReveal = set;
    self._tableView.scrollEnabled = !set;
    [self._tableView setAllowsSelection:!set];
}

- (InfinitestFile*) getValueAtIndex:(NSInteger) index {
    return ([files objectAtIndex:index]);
}

- (NSInteger) numberOfRowsInTableView:(UITableView *)tableView {
    return [files count];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [files count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *SimpleTableIdentifier = @"FilesTableIdentifier";

    NSUInteger row = [indexPath row];
    InfinitestFile *ifFile = [self getValueAtIndex:row];
    
    if (self->displayTextInputCell && row == 0) {
        UITableViewCell* cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:@"textInputCell"];
        
        if (self->dirTextInput)
            cell.imageView.image = folderImage;
        else if (self->fileTextInput) 
            cell.imageView.image = plusImage;
        
        UITextField *txtField=[[UITextField alloc]initWithFrame:CGRectMake(/*5*/ cell.imageView.image.size.width + 5, 10, 310 - cell.imageView.image.size.width - 5, 34)];
        txtField.autoresizingMask=UIViewAutoresizingFlexibleHeight;
        txtField.autoresizesSubviews=YES;
        [txtField setBorderStyle:UITextBorderStyleNone];
        [txtField setPlaceholder:@"Name"];
        [txtField setKeyboardType:UIKeyboardTypeNamePhonePad];
        txtField.returnKeyType = UIReturnKeyDone;

        [cell.contentView addSubview:txtField];
        txtField.delegate = self;
        [txtField becomeFirstResponder];
        
        return cell;
    }
    else {
    ZKRevealingTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:SimpleTableIdentifier];
    if (cell == nil) {
        cell = [[ZKRevealingTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:SimpleTableIdentifier];
        cell.delegate = self;
        //cell.backView.backgroundColor = [UIColor scrollViewTexturedBackgroundColor];
        cell.backView.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"InnerShadowBack.png"]];
        
        cell.direction = ZKRevealingTableViewCellDirectionLeft;
        
       // [cell drawInnerShadowInRect:cell.bounds fillColor:[UIColor blackColor]];

        UIButton *deleteButton = [UIButton buttonWithType:UIButtonTypeCustom]; //UIButtonTypeRoundedRect];
        //[button setTitle:@"X" forState:UIControlStateNormal];
        deleteButton.frame = CGRectMake(cell.backView.bounds.size.width - 100,
                                  cell.backView.bounds.origin.y,
                                  100,
                                  cell.backView.bounds.size.height - 1);

        [deleteButton setImage:[UIImage imageNamed:@"delete.png"] forState:UIControlStateNormal];
        [deleteButton addTarget:self action:@selector(deleteRow:) forControlEvents:UIControlEventTouchUpInside];
        [cell.backView addSubview:deleteButton];
        
        UIButton *emailButton = [UIButton buttonWithType:UIButtonTypeCustom]; //UIButtonTypeRoundedRect];
        //[button setTitle:@"X" forState:UIControlStateNormal];
        emailButton.frame = CGRectMake(cell.backView.bounds.size.width - 180,
                                  cell.backView.bounds.origin.y,
                                  100,
                                  cell.backView.bounds.size.height - 1);
        
        [emailButton setImage:[UIImage imageNamed:@"mail.png"] forState:UIControlStateNormal];
        [emailButton addTarget:self action:@selector(oPresentSendEmailFile:) forControlEvents:UIControlEventTouchUpInside];
        if (ifFile.isDirectory) {
            emailButton.enabled = NO;
        }
        [cell.backView addSubview:emailButton];
        
    }

    cell.textLabel.text = ifFile.dic.name;
    if ([ifFile isDirectory]) {
        cell.imageView.image = [UIImage imageNamed:@"folder.png"];
    }
    else
        cell.imageView.image = [ifFile.dic.icons objectAtIndex:1];
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    if (ifFile.isDirectory) {
        //cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    }
            return cell;
    }
}

- (BOOL)cellShouldReveal:(ZKRevealingTableViewCell *)cell {
    if (self->lockReveal)
        return NO;
    if (self.currentlyRevealedCell != cell)
    {
        [self.currentlyRevealedCell setRevealing:NO];
        self.currentlyRevealedCell = cell;
    }
    return YES;
}

- (void)cellDidReveal:(ZKRevealingTableViewCell *)cell {
    self.currentlyRevealedCell = cell;
}

- (BOOL)loadMoreFiles:(id)sender
{
    NSString*       fileURL;
    NSInteger       i = 0;
    
    while ((i++ < FILES_LIST_BATCH_SIZE) && (fileURL = [nextFile nextObject])) {
        [files addObject:[[InfinitestFile alloc] initWithFileURL:[filesDirectory stringByAppendingPathComponent:fileURL] dicDelegate:DICDelegate]];   
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
            if (![textField.text isEqualToString:@""] && ![self.fileMgr fileExistsAtPath:[filesDirectory stringByAppendingPathComponent:[textField.text stringByAppendingString:@".png"]]]) {
               // NSData* imageData = [NSData dataWithData:UIImagePNGRepresentation(self.imageToSave)];
                //[imageData writeToFile:[textField.text stringByAppendingString:@".png"] atomically:NO]; 
                [self saveData:[NSData dataWithData:UIImagePNGRepresentation(self.imageToSave)] toFile:[textField.text stringByAppendingString:@".png"]];
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
    if (lockReveal)
        lockReveal = NO;
    else {
        [self.currentlyRevealedCell setRevealing:NO];
        self.currentlyRevealedCell = nil;
        lockReveal = YES;
    }
}

-(void) saveData:(NSData*) data toFile:(NSString*) name {
    NSString* path = [filesDirectory stringByAppendingPathComponent:name];

    [data writeToFile:path atomically:NO];
    [files insertObject:[[InfinitestFile alloc] initWithFileURL:path dicDelegate:DICDelegate] atIndex:0];

    [self._tableView reloadData];
}

@end
