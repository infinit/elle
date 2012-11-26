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

/**
 * @brief Update a transaction notification row with a new status.
 */
- (void)updateTransactionStatus:(IATransactionStatusNotification*)notif;

/**
 * @brief Triggered by a pending transaction notification.
 */
- (IBAction)on_accept:(id)sender;
- (IBAction)on_reject:(id)sender;
- (IBAction)on_cancel:(id)sender;

@end
