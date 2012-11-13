//
//  IANotificationListDelegate.m
//  FinderWindowApplication
//
//  Created by infinit on 11/13/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IANotificationListDelegate.h"
#import "IAGapState.h"

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
    NSLog(@"BIM!");
    NSTableCellView *result = [table_view makeViewWithIdentifier:table_column.identifier owner:self];
    NSString* str = @"Wuttt ?!";
    id item = [_notifications objectAtIndex:row];
    if ([item isKindOfClass:[IATransactionStatusNotification class]])
    {
        str = @"NOTIF STATUS";
    }
    else if ([item isKindOfClass:[IATransactionNotification class]])
    {
        str = @"NOTIF";
    }
    
    result.textField.stringValue = str;

    return result;
}

@end
