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
- (void) freeze
{
    [self _freezeActions];
}

- (void)_freezeActions
{
    [self.accept_button setEnabled:FALSE];
    [self.reject_button setEnabled:FALSE];
    [self.accept_button setHidden:FALSE];
    [self.reject_button setHidden:FALSE];
}

- (void)_hideActions
{
    [self.accept_button setEnabled:FALSE];
    [self.reject_button setEnabled:FALSE];
    [self.accept_button setHidden:TRUE];
    [self.reject_button setHidden:TRUE];
}

- (void)_enableActions
{
    [self.accept_button setEnabled:TRUE];
    [self.reject_button setEnabled:TRUE];
    [self.accept_button setHidden:FALSE];
    [self.reject_button setHidden:FALSE];
}

- (void) updateWithStatus:(gap_TransactionStatus)status
{
    [self _update:status];
}

- (void)_update:(gap_TransactionStatus)status
{
    NSLog(@"Updating transaction %@ with status == %d", _notification.transaction_id, status);
    NSString* str = nil;
    switch (status)
    {
        case gap_transaction_status_accepted:
            str = @"transaction accepted.";
            [self _hideActions];
            break;

        case gap_transaction_status_pending:
            str = @"transaction pending.";
            [self _enableActions];
            break;

        case gap_transaction_status_finished:
            str = @"transaction finished.";
            [self _hideActions];
            break;

        case gap_transaction_status_rejected:
            str = @"transaction rejected.";
            [self _hideActions];
            break;

        case gap_transaction_status_started:
            str = @"transaction started.";
            [self _hideActions];
            break;

        default:
            str = [[NSString alloc] initWithFormat:@"Unknown transaction status %d", status];
            [self _hideActions];
            break;
    }
    [self.status setStringValue:str];
    NSLog(@"Update transaction status to %@", str);
}

- (void) update:(id)status_notif
{
    assert(status_notif != nil);
    assert([status_notif isKindOfClass:[IATransactionStatusNotification class]]);
    IATransactionStatusNotification* notif = status_notif;
    [self _update:notif.status];
}

- (void) setNotification:(id)notif
{
    assert(notif != nil);
    assert([notif isKindOfClass:[IATransactionNotification class]]);
    _notification = notif;
    NSString* str;
    if ([_notification.sender_id isEqualToString:[IAClientGapState gap_instance].self_id])
    {
        // COMPARE SENDER DEVICE ID HERE

        // We sent a file to somebody
        if (_notification.files_count == 1)
            str = [[NSString alloc] initWithFormat:@"%@ has not yet accepted the file %@"
                   , _notification.recipient_fullname
                   , _notification.first_filename];
        else
            str = [[NSString alloc] initWithFormat:@"%@ has not yet accepteted the %lu files (%@, ...)"
                   , _notification.recipient_fullname
                   , _notification.files_count
                   , _notification.first_filename];
    }
    else
    {
        // Someone sent us some files
        if (_notification.files_count == 1)
            str = [[NSString alloc] initWithFormat:@"%@ wants to share %@ with you"
                   , _notification.sender_fullname
                   , _notification.first_filename];
        else
            str = [[NSString alloc] initWithFormat:@"%@ wants to share %lu files with you (%@, ...)"
                   , _notification.sender_fullname
                   , _notification.files_count
                   , _notification.first_filename];
    }
    self.textField.stringValue = [[NSString alloc] initWithFormat:@"%@: %@",_notification.transaction_id, str];
    [self _update:_notification.status];
}

@end
