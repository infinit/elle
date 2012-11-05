//
//  InfinitestData.h
//  Infinitest
//
//  Created by Simon Peccaud on 3/20/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface InfinitestData : NSObject <UITableViewDataSource> {
   // NSMutableArray                                              *_files;
   // NSFileManager                                               *_fileMgr;
    //NSString                                                    *_documentsDirectory;
    //id <UIDocumentInteractionControllerDelegate>                _DICDelegate;
}




- (id)                  initWithDocumentInteractionControllerDelegate: (id <UIDocumentInteractionControllerDelegate>) dic;
- (void)                updateFromDirectory;
- (void)                addFile;

- (UIDocumentInteractionController*)           getValueAtIndex:(NSInteger) index;
- (NSInteger)           numberOfRowsInTableView:(UITableView *)tableView;
- (NSInteger)           tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section;
- (UITableViewCell*)   tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath;

@end
