//
//  InfinitestFile.h
//  Infinitest
//
//  Created by Simon Peccaud on 3/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface InfinitFile : NSObject <NSURLConnectionDelegate>

@property (strong, nonatomic) UIDocumentInteractionController*  dic;
@property BOOL  isDirectory;
@property NSNumber  *progress;
@property(strong) NSNumber         *filesize;
@property BOOL isDownloading;
@property (strong) UILabel *fileNameLabel;
@property(strong) UILabel   *progressLabel;
@property(weak) UITableView     *tableView;
@property(strong) NSURLConnection *dlConnection;
@property BOOL isChecked;
@property UIImageView *checkStatusView;

- (id) initWithFileURL:(NSString*)fileURL dicDelegate:(id <UIDocumentInteractionControllerDelegate>)dicDelegate;
- (void) startDownloadWithFilePath:(NSString*)path tableView:(UITableView*) tblView;
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response;
- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data;

@end