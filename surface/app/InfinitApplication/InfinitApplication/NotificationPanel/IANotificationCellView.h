//
//  IANotificationCellView.h
//  FinderWindowApplication
//
//  Created by infinit on 11/14/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "IAGapState.h"

@interface IANotificationCellView : NSTableCellView

- (void)freeze;

@end

@interface IATransactionNotificationCellView : IANotificationCellView

@property gap_TransactionStatus transaction_status;

@property (retain) IBOutlet NSButton* accept_button;
@property (retain) IBOutlet NSButton* reject_button;

@end

@interface IATransactionStatusNotificationCellView : IANotificationCellView

@end
