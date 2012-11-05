//
//  InfinitestFile.h
//  Infinitest
//
//  Created by Simon Peccaud on 3/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface InfinitestFile : NSObject

@property (strong, nonatomic) UIDocumentInteractionController*  dic;
@property BOOL  isDirectory;

- (id) initWithFileURL:(NSString*)fileURL dicDelegate:(id <UIDocumentInteractionControllerDelegate>)dicDelegate;

@end
