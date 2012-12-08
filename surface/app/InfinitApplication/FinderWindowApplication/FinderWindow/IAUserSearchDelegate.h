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
#import "IASearchResultsTableView.h"

@interface IAUserSearchDelegate : NSObject <NSTableViewDelegate, NSTableViewDataSource>

@property (retain) IBOutlet IAUserSearchBar*            search_bar;
@property (retain) IBOutlet NSWindow*                   main_window;
@property (retain) IBOutlet IAMainViewController*       main_view_controller;
@property (retain) IASearchResultsWindow*               search_window;
@property (nonatomic, readonly) IASearchResultsTableView*   table_view;

@end
