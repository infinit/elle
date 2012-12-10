//
//  IASearchResultsWindow.h
//  FinderWindow
//
//  Created by Patrick Perlmutter on 12/6/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "IASearchResultsTableView.h"

@interface IASearchResultsWindow : NSWindow

- (id)initWithParent:(NSWindow*)parent;

- (void)updatePosition:(NSRect)pos;

- (void)hide;
- (void)show;

- (void)moveSelectionDown;
- (void)moveSelectionUp;

@property (nonatomic, readonly) BOOL hidden;
@property (nonatomic, readonly) IASearchResultsTableView* table_view;
@end
