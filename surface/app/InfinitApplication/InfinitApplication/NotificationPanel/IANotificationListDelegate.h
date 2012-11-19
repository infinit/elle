//
//  IANotificationListDelegate.h
//  FinderWindowApplication
//
//  Created by infinit on 11/13/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "IAClientGapState.h"

@interface IANotificationListDelegate : NSObject <NSTableViewDelegate/*, NSTableViewDataSource*/>

- (void)addNotification:(id)notification;

// Number of rows.
- (NSInteger)numberOfRowsInTableView:(NSTableView*)table_view;

- (void)updateTransactionStatus:(IATransactionStatusNotification*)notif;
- (IBAction)on_accept:(id)sender;
- (IBAction)on_reject:(id)sender;

@end
