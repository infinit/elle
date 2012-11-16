//
//  IANotificationCellView.m
//  FinderWindowApplication
//
//  Created by infinit on 11/14/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IANotificationCellView.h"

@implementation IANotificationCellView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)freeze {}

@end

@implementation IATransactionNotificationCellView

@synthesize transaction_status = _transaction_status;

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.transaction_status = gap_transaction_status_none;
    }
    
    return self;
}
- (void)freeze
{
    [self.accept_button setEnabled:FALSE];
    [self.reject_button setEnabled:FALSE];
}

@end


@implementation IATransactionStatusNotificationCellView

@end