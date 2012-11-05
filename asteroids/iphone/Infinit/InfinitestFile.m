//
//  InfinitestFile.m
//  Infinitest
//
//  Created by Simon Peccaud on 3/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "InfinitestFile.h"

@implementation InfinitestFile

@synthesize dic;
@synthesize isDirectory;

- (id) initWithFileURL:(NSString*)fileURL dicDelegate:(id <UIDocumentInteractionControllerDelegate>)dicDelegate
{
    if (![[NSFileManager defaultManager] fileExistsAtPath:fileURL isDirectory:&isDirectory]) {
        return nil;
    };
    
    dic = [UIDocumentInteractionController interactionControllerWithURL:[NSURL  fileURLWithPath:fileURL]];
    
    if (dic) {
        dic.delegate = dicDelegate;
    }
    
    return self;
}

@end
