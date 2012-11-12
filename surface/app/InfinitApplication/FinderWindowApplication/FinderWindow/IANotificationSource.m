//
//  IANotificationSource.m
//  FinderWindowApplication
//
//  Created by infinit on 11/12/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IANotificationSource.h"

@implementation IANotificationSource
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

// Retrieve row value.
- (id)                      tableView:(NSTableView*)table_view
            objectValueForTableColumn:(NSTableColumn*)table_column
                                  row:(NSInteger)index
{
    return [_notifications objectAtIndex:index];    
}

@end
