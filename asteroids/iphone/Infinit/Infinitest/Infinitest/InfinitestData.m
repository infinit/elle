//
//  InfinitestData.m
//  Infinitest
//
//  Created by Simon Peccaud on 3/20/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import "InfinitestData.h"

@interface InfinitestData () {}     // Class extension with private properties
    @property(strong, nonatomic) NSMutableArray                                             *files;
    @property(strong, nonatomic) NSFileManager                                              *fileMgr;
    @property(strong, nonatomic) NSString                                                   *filesDirectory;
    @property(strong, nonatomic) id <UIDocumentInteractionControllerDelegate>                DICDelegate;
@end


@implementation InfinitestData

@synthesize files;
@synthesize fileMgr;
@synthesize filesDirectory;
@synthesize DICDelegate;

- (id)init
{
    self = [super init];

    return self;
}

- (id)                  initWithDocumentInteractionControllerDelegate: (id <UIDocumentInteractionControllerDelegate>) dic
{
    self = [super init];
    if (self) {
        DICDelegate = dic;

        files = [[NSMutableArray alloc] init];
        
        // Create file manager
        fileMgr = [NSFileManager defaultManager];
        
        // Point to Document directory
        filesDirectory = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
        
        [self updateFromDirectory];
    }
    
    return self;
}

- (void) updateFromDirectory {
    NSError*            error;
    NSArray*            array = [fileMgr contentsOfDirectoryAtPath:filesDirectory error:&error];
    NSEnumerator*       e = [array objectEnumerator];
    NSString*           fileURL;

    while (fileURL = [e nextObject]) {
        NSLog(@"file = %@", [filesDirectory stringByAppendingPathComponent:fileURL]);
        UIDocumentInteractionController *interactionController = [UIDocumentInteractionController interactionControllerWithURL:[NSURL fileURLWithPath:[filesDirectory stringByAppendingPathComponent:fileURL]]];
        
        interactionController.delegate = DICDelegate;
        
        [files addObject:interactionController];   
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
    
    // Show contents of Documents directory
    NSLog(@"Documents directory (%@): %@",
          filesDirectory,
          [fileMgr contentsOfDirectoryAtPath:filesDirectory error:&error]);
    
    [files addObject:[@"file" stringByAppendingFormat:@"%d.txt", [files count]]];
}

- (UIDocumentInteractionController*) getValueAtIndex:(NSInteger) index {
    UIDocumentInteractionController *interactionController = [files objectAtIndex:index];
    return (interactionController);
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

    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:SimpleTableIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc]initWithStyle:UITableViewCellStyleDefault reuseIdentifier:SimpleTableIdentifier];
    }
    NSUInteger row = [indexPath row];
    cell.textLabel.text = [self getValueAtIndex:row].name;
    return cell;
}


@end
