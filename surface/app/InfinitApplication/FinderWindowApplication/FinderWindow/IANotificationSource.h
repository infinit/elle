//
//  IANotificationSource.h
//  FinderWindowApplication
//
//  Created by infinit on 11/12/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface IANotificationSource : NSObject <NSTableViewDataSource>

- (void)addNotification:(id)notification;

// Number of rows.
- (NSInteger)numberOfRowsInTableView:(NSTableView*)table_view;

// Retrieve row value.
- (id)                      tableView:(NSTableView*)table_view
            objectValueForTableColumn:(NSTableColumn*)table_column
                                  row:(NSInteger)index;

@end
