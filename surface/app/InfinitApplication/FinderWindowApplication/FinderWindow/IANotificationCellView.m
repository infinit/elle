//
//  IANotificationCellView.m
//  FinderWindowApplication
//
//  Created by infinit on 11/14/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IANotificationCellView.h"
#import "IAClientGapState.h"

@implementation IANotificationCellView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void) freeze {assert(false);}
- (void) update:(id)status_notif {assert(false);}
- (void) setNotification:(id)notif {assert(false);}

@end

@implementation IATransactionNotificationCellView
{
@private
    IATransactionNotification* _notification;
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _notification = nil;
    }
    return self;
}

- (void)freeze
{
    [self.accept_button setEnabled:FALSE];
    [self.reject_button setEnabled:FALSE];
}

- (void) update:(id)status_notif
{
    assert(status_notif != nil);
    assert([status_notif isKindOfClass:[IATransactionStatusNotification class]]);
    IATransactionStatusNotification* notif = status_notif;
    NSLog(@"Updating transaction notif with status == %d", notif.status);
    NSString* str = nil;
    switch (notif.status)
    {
        case gap_transaction_status_accepted:
            str = @"transaction accepted.";
            break;
            
        case gap_transaction_status_pending:
            str = @"transaction pending.";
            break;

        case gap_transaction_status_finished:
            str = @"transaction finished.";
            break;
            
        case gap_transaction_status_rejected:
            str = @"transaction rejected.";
            break;
        case gap_transaction_status_started:
            str = @"transaction started.";
            break;
            
        default:
            str = [[NSString alloc] initWithFormat:@"Unknown transaction status %d", notif.status];
            break;
    }
    NSLog(@"Update transaction status to %@", str);
    [self.status setStringValue:str];
}

- (void) setNotification:(id)notif
{
//assert(_notification == nil);
    assert(notif != nil);
    assert([notif isKindOfClass:[IATransactionNotification class]]);
    _notification = notif;
    [self.status setStringValue:@"Pending"]; //XXX
}

@end
