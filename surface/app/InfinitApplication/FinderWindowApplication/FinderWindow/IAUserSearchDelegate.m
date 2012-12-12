//
//  IAUserSearchBarDelegate.m
//  FinderWindow
//
//  Created by Patrick Perlmutter on 12/6/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAUserSearchDelegate.h"
#import "IAGapState.h"

@implementation IAUserSearchDelegate
{
@private
    NSMutableArray* _users;
    NSOperation*    _current_search;
}

- (IASearchResultsTableView*)table_view
{
    return self.search_window.table_view;
}

-(id)init
{
    if (self = [super init])
    {
        _users = nil;
        _current_search = nil;
    }
    return self;
}
- (void)awakeFromNib
{
    NSLog(@"Awake from nib, cell: %@, window: %@", self.search_bar.cell, self.main_window);
    [self.search_bar.cell setSendsWholeSearchString:NO];
    [self.search_bar.cell setTarget:self];
    [self.search_bar.cell setAction:@selector(updateFilter:)];
    NSLog(@"CREATINGSEARCHRESULTSWINDOW");
    self.search_window = [[IASearchResultsWindow alloc] initWithParent:self.main_window];
    [self.search_window updatePosition:[self _getWindowPosition]];
    NSLog(@"window: %@, Tableview: %@", self.search_window, self.search_window.table_view);
    [self.table_view setDataSource:self];
    [self.table_view setDelegate:self];
    [self.table_view reloadData];
}

- (NSRect)_getWindowPosition
{
    NSRect pos;
    pos.origin = self.main_window.frame.origin;
    pos.origin.x += self.search_bar.frame.origin.x;
    pos.origin.y += self.search_bar.frame.origin.y;
    pos.size = self.search_bar.frame.size;
    pos.size.height = 400;
//    pos.size.width = 600;
    return pos;
}

- (void)updateFilter:(id)sender
{

    NSLog(@"search for %@", [self.search_bar.cell stringValue]);
    NSString* str = [self.search_bar.cell stringValue];
    if (_current_search != nil)
    {
        [_current_search cancel];
        _current_search = nil;
    }
    
    [self.main_view_controller refresh];

    if (str && [str length])
    {
        _current_search = [[IAGapState instance] searchUsers:str
                                             performSelector:@selector(_onUserResults:)
                                                    onObject:self];
        [self.search_window updatePosition:[self _getWindowPosition]];
        [self.search_window show];
        [self.table_view reloadData];
        [self.table_view setNeedsDisplay:YES];
    }
    else
        [self.search_window hide];
}

- (void)_onUserResults:(IAGapOperationResult*)result
{
    _current_search = nil;
    if (result.error)
    {
        NSLog(@"Couldn't search users: %d", result.status);
    }
    NSString* str = [self.search_bar.cell stringValue];
    if (str && [str length])
    {
        assert([result.data isKindOfClass:[NSMutableArray class]]);
        _users = (NSMutableArray*)result.data;
        [self.table_view reloadData];
        [self.table_view selectFirstRow];
    }
    else
        [self.search_window hide];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView*)table_view
{
    return [_users count];
}


- (NSView*)         tableView:(NSTableView*)table_view
           viewForTableColumn:(NSTableColumn*)table_column
                          row:(NSInteger)row
{
        NSLog(@"Fetch view for table view %@", table_view);
    IAUser* user = [_users objectAtIndex:row];
    if (user  == nil)
        return nil;
    
    NSString* cell_id = [table_column identifier];
    NSLog(@"Column identifier: %@", cell_id);
    NSTextField *result = [table_view makeViewWithIdentifier:cell_id owner:self];
    
    if (result == nil)
    {
        NSLog(@"Creating a table cell template for identifier %@", cell_id);
        result = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 10, 10)];
        result.identifier = cell_id;
    }

//    [result setStringValue:[NSString stringWithFormat:@"row: %ld", row]];
    [result setStringValue:[NSString stringWithFormat:@"%@ (%@)", user.fullname, user.email]];
    
    [result setBackgroundColor:[NSColor clearColor]];
    NSLog(@"Return row %@", result);
    return result;
}

- (BOOL)        control:(NSControl *)control
               textView:(NSTextView*)text_view
    doCommandBySelector:(SEL)command
{
    if (control != self.search_bar)
        return NO;
    
    if (command == @selector(moveUp:))
    {
        [self.table_view moveSelectionUp];
    }
    else if (command == @selector(moveDown:))
    {
        [self.table_view moveSelectionDown];
    }
    else if (command == @selector(insertNewline:))
    {
        [self.search_window hide];
    }
    else
    {
        return NO;
    }
    return YES;
}

//-(id)           tableView:(NSTableView *)aTableView
//objectValueForTableColumn:(NSTableColumn *)aTableColumn
//                      row:(NSInteger)rowIndex
//{
//    NSLog(@"OBJECT VALUE");
//    return @"toto";
//}

- (NSTableRowView *)tableView:(NSTableView *)tableView rowViewForRow:(NSInteger)row
{
    NSLog(@"Row view");
    return nil;
}

@end
