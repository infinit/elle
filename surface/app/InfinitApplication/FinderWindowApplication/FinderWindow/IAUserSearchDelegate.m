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
}

- (void)awakeFromNib
{
    NSLog(@"Awake from nib, cell: %@, window: %@", self.search_bar.cell, self.main_window);
    [self.search_bar.cell setSendsWholeSearchString:NO];
    [self.search_bar.cell setTarget:self];
    [self.search_bar.cell setAction:@selector(updateFilter:)];
    self.search_window = [[IASearchResultsWindow alloc] initWithParent:self.main_window
                                                          andTableView:self.table_view];
    [self.search_window updatePosition:[self _getWindowPosition]];
  //  [self.table_view setDataSource:self];
  //  [self.table_view setDelegate:self];
}

- (NSRect)_getWindowPosition
{
    NSRect pos;
    pos.origin = self.main_window.frame.origin;
    pos.origin.x += self.search_bar.frame.origin.x;
    pos.origin.y += self.search_bar.frame.origin.y;
    pos.size = self.search_bar.frame.size;
    pos.size.height = 400;
    pos.size.width = 600;
    return pos;
}

- (void)updateFilter:(id)sender
{

    NSLog(@"search for %@", [self.search_bar.cell stringValue]);
    NSString* str = [self.search_bar.cell stringValue];
    
    [self.main_view_controller refresh];

    if (str && [str length])
    {
        [self.search_window updatePosition:[self _getWindowPosition]];
        [self.search_window show];
        [self.table_view reloadData];
        [self.table_view setNeedsDisplay:YES];
    }
    else
        [self.search_window hide];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView*)table_view
{
    NSLog(@"COUNT!");
    return 2;
    return [_users count];
}


- (NSView*)         tableView:(NSTableView*)table_view
           viewForTableColumn:(NSTableColumn*)table_column
                          row:(NSInteger)row
{
        NSLog(@"Fetch view !");
//    IAUser* user = [_users objectAtIndex:row];
//    if (user  == nil)
//        return nil;
    
    NSString* cell_id = @"IAUserSearchTableCellView";
    NSTableCellView *result = [table_view makeViewWithIdentifier:cell_id owner:self];
    
    // TODO set user here
    [result.textField setStringValue:[NSString stringWithFormat:@"row %li", row]];
    
    [result.textField setBackgroundColor:[NSColor redColor]];
    NSLog(@"Return row %@", result);
    return result;
}

-(id)           tableView:(NSTableView *)aTableView
objectValueForTableColumn:(NSTableColumn *)aTableColumn
                      row:(NSInteger)rowIndex
{
    NSLog(@"OBJECT VALUE");
    return @"toto";
}

- (NSTableRowView *)tableView:(NSTableView *)tableView rowViewForRow:(NSInteger)row
{
    NSLog(@"Row view");
    return nil;
}

@end
