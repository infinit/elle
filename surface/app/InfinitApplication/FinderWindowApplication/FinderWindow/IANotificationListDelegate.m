//
//  IANotificationListDelegate.m
//  FinderWindowApplication
//
//  Created by infinit on 11/13/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IANotificationListDelegate.h"
#import "IANotificationCellView.h"

@interface IANotificationListDelegate ()

@property IBOutlet NSTableView* table;

@end

@implementation IANotificationListDelegate
{
@private
    NSMutableArray* _notifications;
}

- (id)init
{
    self = [super init];
    if (self)
    {
        _notifications = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)addNotification:(id)notification
{
    [_notifications insertObject:notification atIndex:0];
    NSLog(@"ADD: source %@ size: %lu", self, [_notifications count]);
}

// Number of rows.
- (NSInteger)numberOfRowsInTableView:(NSTableView*)table_view
{
    NSLog(@"GET: source %@ size: %lu", self, [_notifications count]);
    return [_notifications count];
}

//// Retrieve row value.
//- (id)                      tableView:(NSTableView*)table_view
//            objectValueForTableColumn:(NSTableColumn*)table_column
//                                  row:(NSInteger)index
//{
//    return [_notifications objectAtIndex:index];
//}

- (NSView*)         tableView:(NSTableView*)table_view
           viewForTableColumn:(NSTableColumn*)table_column
                          row:(NSInteger)row
{
    NSString* str = nil;
    IANotificationCellView *result = nil;
    id item = [_notifications objectAtIndex:row];
    if ([item isKindOfClass:[IATransactionStatusNotification class]])
    {
        IATransactionStatusNotification* notif = item;
        switch (notif.status)
        {
            case gap_transaction_status_accepted:
                str = @"transaction accepted.";
                break;
                
            case gap_transaction_status_pending:
                str = @"transaction pending.";
                break;
                
            case gap_transaction_status_ready:
                str = @"transaction ready.";
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
        result = [table_view makeViewWithIdentifier:@"TransactionStatusNotificationCellView" owner:self];
    }
    else if ([item isKindOfClass:[IATransactionNotification class]])
    {
        IATransactionNotification* notif = item;
        if (notif.files_count == 1)
            str = [[NSString alloc] initWithFormat:@"%@ wants to share %@ with you"
                                                  , notif.sender_fullname
                                                  , notif.first_filename];
        else
            str = [[NSString alloc] initWithFormat:@"%@ wants to share %lu files with you (%@, ...)"
                                                  , notif.sender_fullname
                                                  , notif.files_count
                                                  , notif.first_filename];
        result = [table_view makeViewWithIdentifier:@"TransactionNotificationCellView" owner:self];
    }
    else
    {
        NSLog(@"WARNING: Unexpected notification type");
        return nil;
    }

//    IANotificationCellView *result = [table_view makeViewWithIdentifier:table_column.identifier owner:self];
    result.textField.stringValue = str;

    return result;
}


- (void)updateTransactionStatus:(IATransactionStatusNotification*)notif
{
    NSInteger i = 0, row = -1;
    for (id n in _notifications)
    {
        if ([n isKindOfClass:[IATransactionNotification class]])
        {
            IATransactionNotification* tr_n;
            if (tr_n.transaction_id == notif.transaction_id)
            {
                row = i;
                break;
            }
        }
        i++;
    }
    if (row != -1)
    {
        IANotificationCellView* view = [self.table viewAtColumn:0 row:row makeIfNecessary:NO];
        if (view == nil)
            return;
        
    }
}

- (IBAction)on_accept:(id)sender
{
    NSUInteger row = [self.table rowForView:sender];
    IANotificationCellView* view = [self.table viewAtColumn:0 row:row makeIfNecessary:NO];
    
    id item = [_notifications objectAtIndex:row];
    assert([item isKindOfClass:[IATransactionNotification class]]);
    IATransactionNotification* notif = item;
    NSLog(@"accept for view: %@", view);
    [view freeze];
    [[IAClientGapState gap_instance] acceptTransaction:notif
                                       performSelector:@selector(_on_accept_done:)
                                              onObject:self];
}

- (void)_on_accept_done:(IAGapOperationResult*)op
{
    NSLog(@"ACCEPT DONE !");
    if (op.success)
    {
        NSLog(@"SUCCESSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS");
    }
}

- (IBAction)on_reject:(id)sender
{
    NSLog(@"reject");
    NSUInteger row = [self.table rowForView:sender];
    IANotificationCellView* view = [self.table viewAtColumn:0 row:row makeIfNecessary:NO];
    
    id item = [_notifications objectAtIndex:row];
    assert([item isKindOfClass:[IATransactionNotification class]]);
    IATransactionNotification* notif = item;
    NSLog(@"reject for view: %@", view);
    [view freeze];
    [[IAClientGapState gap_instance] rejectTransaction:notif
                                       performSelector:@selector(_on_reject_done:)
                                              onObject:self];
}

- (IBAction)_on_reject_done:(IAGapOperationResult*)op
{
    if (op.success)
    {
        NSLog(@"Reject with success");
    }
}

@end
