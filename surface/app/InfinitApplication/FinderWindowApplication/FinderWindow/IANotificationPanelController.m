//
//  IANotificationPanelController.m
//  FinderWindowApplication
//
//  Created by infinit on 11/9/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IANotificationPanelController.h"
#import "IANotificationPanel.h"
#import "IAClientGapState.h"
#import "IANotificationCellView.h"

@implementation IANotificationPanelController
{
@private
    BOOL _visibility;
}

- (void)awakeFromNib
{
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(_onTransactionNotification:)
                                                 name:IA_GAP_EVENT_TRANSACTION_NOTIFICATION
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(_onTransactionStatusNotification:)
                                                 name:IA_GAP_EVENT_TRANSACTION_STATUS_NOTIFICATION
                                               object:nil];
}

- (void)_onTransactionNotification:(NSNotification*)n;
{
    assert([[n object] isKindOfClass:[IATransaction class]]);
    IATransaction* transaction = [n object];
    NSLog(@"ON TRANSACTION NOTIFICATION %@", transaction.transaction_id);
    [[self source] addTransaction:transaction];
    [self.table reloadData];
}

- (void)_onTransactionStatusNotification:(NSNotification*)n
{
    NSLog(@"ON TRANSACTION STATUS NOTIFICATION");
    assert([[n object] isKindOfClass:[IATransaction class]]);
    IATransaction* notif = [n object];
    [self.source updateTransactionStatus:notif];
    [self.table reloadData];
}

- (BOOL)visibility
{
    return _visibility;
}

- (void)toggleVisibility
{
    [self setVisibility:![self visibility]];
}

- (void)setVisibility:(BOOL)visibility
{
    _visibility = visibility;
    if (visibility)
    {
        [NSApp activateIgnoringOtherApps:YES];
        [[self panel] makeKeyAndOrderFront:self];
        [[self panel] setOrderedIndex:0];
        [[self panel] show];
    }
    else
        [[self panel] hide];
}

- (IANotificationPanel*)panel
{
    return (IANotificationPanel*)[self window];
}
@end
