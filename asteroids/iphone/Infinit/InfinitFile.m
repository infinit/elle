//
//  InfinitestFile.m
//  Infinitest
//
//  Created by Simon Peccaud on 3/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "InfinitFile.h"

@implementation InfinitFile

@synthesize dic;
@synthesize isDirectory;
@synthesize progress;
@synthesize filesize;
@synthesize isDownloading;
@synthesize fileNameLabel;
@synthesize progressLabel;
@synthesize tableView;
@synthesize dlConnection;
@synthesize isChecked;
@synthesize checkStatusView;

- (id) initWithFileURL:(NSString*)fileURL dicDelegate:(id <UIDocumentInteractionControllerDelegate>)dicDelegate
{
    if (![[NSFileManager defaultManager] fileExistsAtPath:fileURL isDirectory:&isDirectory]) {
        return nil;
    };

    dic = [UIDocumentInteractionController interactionControllerWithURL:[NSURL  fileURLWithPath:fileURL]];
    
    if (dic) {
        dic.delegate = dicDelegate;
    }
    
    self.fileNameLabel = [[UILabel alloc] initWithFrame:CGRectMake(45, 12, 270, 20)];
    [self.fileNameLabel setText:self.dic.name];
    
    self.isDownloading = NO;
    self.isChecked = NO;

    return self;
}

- (void) startDownloadWithFilePath:(NSString*)filePath tableView:(UITableView*) tblView{
    self.progress = 0;
    self.filesize = 0;
    self.isDownloading = YES;
    self.tableView = tblView;

    self.progressLabel = [[UILabel alloc] initWithFrame:CGRectMake(280, 12, 40, 20)];
    [self.progressLabel setText:@" 0"];
    self.progressLabel.textColor = [UIColor colorWithHue:20.0 saturation:1 brightness:0 alpha:0.6];

    NSURLRequest *theRequest=[NSURLRequest requestWithURL:[NSURL URLWithString:filePath]
                                              cachePolicy:NSURLRequestUseProtocolCachePolicy
                                          timeoutInterval:60.0];
    
    self.dlConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
 //   [theConnection start];
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
    //    [self.resourceData setLength:0];
    self.filesize = [NSNumber numberWithLongLong:[response expectedContentLength]];
    //    NSLog(@"content-length: %lli bytes", [self.filesize longLongValue]);
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
    
   // NSString* path = [filesDirectory stringByAppendingPathComponent:@"TST.mp4"];
    if (self.progress == 0)
        [data writeToURL:[self.dic URL] atomically:NO];
      //  [data writeToFile:path atomically:NO];
    else {
        // NSLog(@"resourceData length: %d", [self.resourceData length]);
        
        // [self.resourceData appendData:data];
        
        // NSNumber *resourceLength = [NSNumber numberWithUnsignedInteger:[self.resourceData length]];
        
        // NSLog(@"resourceData length: %d", [resourceLength intValue]);
        //  progressView.progress = [resourceLength floatValue] / [self.filesize floatValue];
        
        // [self saveData:resourceData toFile:@"TST.mov"];
        
        NSFileHandle *myHandle = [NSFileHandle fileHandleForUpdatingURL:[self.dic URL] error:nil];
        [myHandle seekToEndOfFile];
        [myHandle writeData:data];
        [myHandle closeFile];
    }
    
    self.progress = [NSNumber numberWithInt:[self.progress intValue] + [data length]];
    
    if ([self.progress floatValue] == [self.filesize floatValue])
    {
        self.isDownloading = NO;
        [self.tableView reloadData];
    }
    else {
        CGFloat progressRatio = [self.progress floatValue] / [self.filesize floatValue];
        [self.progressLabel setText:[NSString stringWithFormat:@" %.0f", progressRatio * 100]];
        self.progressLabel.textColor = [UIColor colorWithHue:20.0 saturation:1 brightness:1/*0.2 + (1 - progressRatio) * 10*/ alpha:0.6 + progressRatio*2];
    }
    //NSLog(@"%f", [self.progress floatValue] / [self.filesize floatValue]);
}


@end
