//
//  IAUserSearchBarDelegate.h
//  FinderWindow
//
//  Created by Patrick Perlmutter on 12/6/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "IAUserSearchBar.h"
#import "IASearchResultsWindow.h"
#import "IAMainViewController.h"

@interface IAUserSearchDelegate : NSObject

@property (assign) IBOutlet IAUserSearchBar*        search_bar;
@property (assign) IBOutlet NSWindow*               main_window;
@property (assign) IBOutlet IAMainViewController*   main_view_controller;
@property (retain) IASearchResultsWindow*           search_window;

@end
