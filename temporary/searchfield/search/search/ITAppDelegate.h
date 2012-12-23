//
//  ITAppDelegate.h
//  search
//
//  Created by infinit on 12/5/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "ITSearchWindow.h"

@interface ITAppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>

@property (assign) IBOutlet NSWindow*       window;
@property (assign) IBOutlet NSSearchField*  search_field;
@property (retain)          ITSearchWindow* search_window;

@end
