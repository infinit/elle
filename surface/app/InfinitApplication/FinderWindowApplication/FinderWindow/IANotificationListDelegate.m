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
    NSMutableArray* _transactions;
}

- (id)init
{
    self = [super init];
    if (self)
    {
        _transactions = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)addTransactionNotification:(id)transaction
{
    IATransaction* tr = transaction;
    if ([self _rowForTransactionId:tr.transaction_id] != -1)
    {
        NSLog(@"Ignoring one more notif for transaction %@", tr.transaction_id);
        return;
    }
    [_transactions insertObject:transaction atIndex:0];
    NSLog(@"ADD: source %@ size: %lu", tr.transaction_id, [_transactions count]);
}

// Number of rows.
- (NSInteger)numberOfRowsInTableView:(NSTableView*)table_view
{
    NSLog(@"GET: source %@ size: %lu", self, [_transactions count]);
    return [_transactions count];
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
    IANotificationCellView *result = nil;
    id item = [_transactions objectAtIndex:row];
    
    IATransaction* transaction = item;
    NSLog(@"SELF ID   = %@",[IAClientGapState gap_instance].self_id);
    NSString* cell_id;
    if ([transaction.sender_id isEqualToString:[IAClientGapState gap_instance].self_id])
        cell_id = @"TransactionNotificationCellViewFromSelf";
    else
        cell_id = @"TransactionNotificationCellView";
    
    result = [table_view makeViewWithIdentifier:cell_id owner:self];
    
    [result setNotification:transaction];

    return result;
}

- (NSInteger)_rowForTransactionId:(NSString*)transaction_id
{
    NSInteger row = 0;
    for (id n in _transactions)
    {
        if ([n isKindOfClass:[IATransaction class]])
        {
            IATransaction* tr_n = n;
            if ([tr_n.transaction_id isEqualToString:transaction_id])
                return row;
            else
                NSLog(@"%@ != %@!", tr_n.transaction_id, transaction_id);
        }
        row++;
    }
    return -1;
}

- (IANotificationCellView*)_viewForTransactionId:(NSString*)transaction_id
{
    NSInteger row = [self _rowForTransactionId:transaction_id];
    if (row != -1)
    {
        return [self.table viewAtColumn:0 row:row makeIfNecessary:NO];
    }
    return nil;
}

- (void)updateTransactionStatus:(IATransaction*)transaction
{
    NSLog(@"Updating transaction status");
    IANotificationCellView* view = [self _viewForTransactionId:transaction.transaction_id];
    if (view != nil)
    {
        [view update:transaction];
    }
    else
    {
        NSLog(@"Cannot find view for transaction id = %@", transaction.transaction_id);
    }
}

- (IBAction)on_accept:(id)sender
{
    NSUInteger row = [self.table rowForView:sender];
    IANotificationCellView* view = [self.table viewAtColumn:0 row:row makeIfNecessary:NO];
    
    id item = [_transactions objectAtIndex:row];
    assert([item isKindOfClass:[IATransaction class]]);
    IATransaction* notif = item;
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
        
    }
}

- (IBAction)on_reject:(id)sender
{
    NSLog(@"reject");
    NSUInteger row = [self.table rowForView:sender];
    IANotificationCellView* view = [self.table viewAtColumn:0 row:row makeIfNecessary:NO];
    
    id item = [_transactions objectAtIndex:row];
    assert([item isKindOfClass:[IATransaction class]]);
    IATransaction* transaction = item;
    NSLog(@"reject for view: %@", view);
    [view freeze];
    [[IAClientGapState gap_instance] rejectTransaction:transaction
                                       performSelector:@selector(_on_reject_done:)
                                              onObject:self];
}

- (void)_on_reject_done:(IAGapOperationResult*)op
{
    assert([op.data isKindOfClass:[IATransaction class]]);
    IATransaction* notif = (IATransaction*)op.data;
    if (op.success)
    {
        NSLog(@"Reject with success");
    }
    else
        [self _removeNotifFor:notif.transaction_id];
}

- (IBAction)on_cancel:(id)sender
{
    NSUInteger row = [self.table rowForView:sender];
    IANotificationCellView* view = [self.table viewAtColumn:0 row:row makeIfNecessary:NO];
    
    id item = [_transactions objectAtIndex:row];
    assert([item isKindOfClass:[IATransaction class]]);
    IATransaction* notif = item;
    NSLog(@"cancel for view: %@", view);
    [view freeze];
    [[IAClientGapState gap_instance] cancelTransaction:notif
                                       performSelector:@selector(_on_cancel_done:)
                                              onObject:self];
}

- (void)_on_cancel_done:(IAGapOperationResult*)op
{
    assert([op.data isKindOfClass:[IATransaction class]]);
    IATransaction* notif = (IATransaction*)op.data;
    if (op.success)
    {
        NSLog(@"Cancel with success");
    }
    else
    {
        NSLog(@"Couldn't cancel transaction_id %@", notif.transaction_id);
    }
    [self _removeNotifFor:notif.transaction_id];
}

- (void)_removeNotifFor:(NSString*)transaction_id
{
    NSInteger row = [self _rowForTransactionId:transaction_id];
    if (row != -1)
    {
        [_transactions removeObjectAtIndex:row];
        [[self table] reloadData];
    }
    else
        NSLog(@"CANNOT FIND ROW FOR TR ID = %@", transaction_id);
}

@end
