//
//  IASearchResultsTableView.h
//  FinderWindow
//
//  Created by Patrick Perlmutter on 12/6/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface IASearchResultsTableView : NSTableView

- (void)selectRow:(NSUInteger)row;
- (void)selectFirstRow;
- (void)moveSelectionDown;
- (void)moveSelectionUp;

@end
