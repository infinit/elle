//
//  InfinitestData.h
//  Infinitest
//
//  Created by Simon Peccaud on 3/20/12.
//  Copyright 2012 infinit.io. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "InfinitFile.h"
#import "ZKRevealingTableViewCell.h"
#import "JSONKit.h"
#import "LoginViewController.h"
#import "InfinitDevices.h"
#import "InfinitNetworks.h"
#import "DriveInfoViewController.h"

// Maximum number of files displayed/fetched before user has to request more (via tableview footer pull)
#define FILES_LIST_BATCH_SIZE   10000
#define ROOT_FILE_DIR           @"Documents/Infinit"
#define PLIST_PATH_COMPONENT    @"Documents/InfinitUInfo.plist"

// Contains file list and info needed by each RootViewController
@interface InfinitData : NSObject < UITableViewDataSource,
                                    ZKRevealingTableViewCellDelegate,
                                    UITextInputDelegate,
                                    UITableViewDelegate>
{
    BOOL displayTextInputCell;
    BOOL dirTextInput;
    BOOL fileTextInput;
    BOOL linkTextInput;
}

@property(strong, nonatomic) NSString       *filesDirectory;                        // directory path
@property(strong, nonatomic) ZKRevealingTableViewCell *currentlyRevealedCell;       // the cell currently revealed (via a swipe to left)
@property(strong)   UIImage   *folderImage;                                         // folder icon
@property(strong)   UIImage   *plusImage;                                           // plus icon displayed when adding a new file/directory
@property(strong)   UIImage*   imageToSave;                                         // a new image that needs to be saved (photo shoot or library import)
@property(strong)   NSURL *movieToSavePath;                                         // path to a new movie to be saved (via movie shoot)
@property           id rootView;                                                    // parent RootView that links this data (used for @selector calls)
@property           UIBarButtonItem          *addViewButtonItem;                    // upper right button for adding directory/file
@property UIImageView *checkView;                                                   // checked square view for selected file
@property UIImageView *uncheckedView;                                               // unchecked square view for unselected file
@property BOOL  editMode;                                                           // are we in edit mode ? (move/copy/pin)
@property BOOL avoidDoubleAction;                                                   // used to avoid multiple unwanted gesture recognizer triggering
@property BOOL rootList;                                                            // are we the data for the first rootview (that means drives listing)

// init with wanted files directory path for this data and global dic
- (id) initWithDocumentInteractionControllerDelegate:(id <UIDocumentInteractionControllerDelegate>) dic
                                                    filesDirectory:(NSString*) filesDir
                                                    tableView:(UITableView*) tableView;
// erase already loaded file and reload them according to FILES_LIST_BATCH_SIZE
- (BOOL)                updateFromDirectory;
// insert a new file (instantiates a new InfinitFile)
- (BOOL) insertFile:(NSString*) name atPosition:(int) row;
// returns the wanted InfinitFile
- (InfinitFile*)                                getValueAtIndex:(NSInteger) index;
// returns the number of files loaded
- (NSInteger)                                   numberOfRowsInTableView:(UITableView *)tableView;
// returns the number of files loaded (only 1 section)
- (NSInteger)                                   tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section;
// returns the cell residing at the specified row
- (UITableViewCell*)                            tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath;
//
- (void) updateFromSearch:(NSString *) searchChars;
- (BOOL)loadMoreFiles:(id)sender;
- (void)resetLoadedFilesCounter;
- (BOOL) addDir:(NSString*) name;
- (void) addDirTextInputCell;
- (void) addFileTextInputCell;
- (void) addLinkTextInputCell;

- (IBAction) deleteRow:(id)sender;
- (IBAction)deleteFile:(InfinitFile*) ifFile;

-(void)textFieldDidEndEditing:(UITextField *)textField;
-(BOOL)textFieldShouldReturn:(UITextField *)textField;

-(void) toggleLockReveal;
-(void) setLockReveal:(BOOL) set;
- (void) toggleInputCell;

- (void) saveData:(NSData*) data toFile:(NSString*) name;

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response;
- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data;

-(void) addFileDownloadFromFilePath: (NSString*) path toFile:(NSString*) filePath;

-(void) insertFilePath:(NSString*) filePath;
- (void) hideCheckMarks;

- (NSMutableArray*) getCheckedFiles;
- (NSMutableArray*) getCheckedData;
- (void) setLockReveal:(BOOL) set;
- (void) clearAllFiles;

- (NSMutableArray*) getDevices;

@end
