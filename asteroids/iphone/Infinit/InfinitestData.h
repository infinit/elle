//
//  InfinitestData.h
//  Infinitest
//
//  Created by Simon Peccaud on 3/20/12.
//  Copyright 2012 infinit.io. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "InfinitestFile.h"
#import "ZKRevealingTableViewCell.h"

#define FILES_LIST_BATCH_SIZE 10

@interface InfinitestData : NSObject <UITableViewDataSource,
                                    ZKRevealingTableViewCellDelegate,
                                    UITextInputDelegate>
{
    BOOL displayTextInputCell;
    BOOL dirTextInput;
    BOOL fileTextInput;
    BOOL lockReveal;
}


@property(strong, nonatomic) NSString       *filesDirectory;
@property(strong, nonatomic) ZKRevealingTableViewCell *currentlyRevealedCell;
@property(strong) UIImage   *folderImage;
@property(strong) UIImage   *plusImage;
@property(strong)               UIImage*   imageToSave;
@property(strong)               NSURL *movieToSavePath;

- (id) initWithDocumentInteractionControllerDelegate:(id <UIDocumentInteractionControllerDelegate>) dic filesDirectory:(NSString*) filesDir tableView:(UITableView*) tableView;
- (BOOL)                updateFromDirectory;
- (void)                addFile;

- (InfinitestFile*)                             getValueAtIndex:(NSInteger) index;
- (NSInteger)                                   numberOfRowsInTableView:(UITableView *)tableView;
- (NSInteger)                                   tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section;
- (UITableViewCell*)                            tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath;

- (void) updateFromSearch:(NSString *) searchChars;
- (BOOL)loadMoreFiles:(id)sender;
- (void)resetLoadedFilesCounter;
- (BOOL) addDir:(NSString*) name;
- (void) addDirTextInputCell;
- (void) addFileTextInputCell;

- (IBAction) deleteRow:(id)sender;

-(void)textFieldDidEndEditing:(UITextField *)textField;
-(BOOL)textFieldShouldReturn:(UITextField *)textField;

-(void) toggleLockReveal;
- (void) toggleInputCell;

-(void) saveData:(NSData*) data toFile:(NSString*) name;

@end
