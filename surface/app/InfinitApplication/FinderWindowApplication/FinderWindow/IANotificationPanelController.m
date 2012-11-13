//
//  IANotificationPanelController.m
//  FinderWindowApplication
//
//  Created by infinit on 11/9/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IANotificationPanelController.h"
#import "IANotificationPanel.h"
#import "IAGapState.h"

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
//    NSLog(@"YEAH I'm there: %@", self.source);
//    for (int i=0; i < 200; ++i)
//        [self.source addNotification:[[NSString alloc] initWithFormat:@"ROW %d", i]];
}

- (void)_onTransactionNotification:(NSNotification*)n;
{
    IATransactionNotification* notif = [n object];
    NSString* str = [NSString alloc];
    if (notif.files_count == 1)
        str = [str initWithFormat:@"%@ wants to share %@ with you"
               , notif.sender_fullname
               , notif.first_filename];
    else
        str = [str initWithFormat:@"%@ wants to share %lu files with you (%@, ...)"
               , notif.sender_fullname
               , notif.files_count
               , notif.first_filename];
    [[self source] addNotification:str];
    [self.table reloadData];
}

- (void)_onTransactionStatusNotification:(NSNotification*)n
{
    IATransactionStatusNotification* notif = [n object];
    
    [[self source] addNotification:notif];
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
        [[self panel] show];
    else
        [[self panel] hide];
}

- (IANotificationPanel*)panel
{
    return (IANotificationPanel*)[self window];
}
@end
