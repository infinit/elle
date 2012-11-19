//
//  IAStatusBarController.m
//  FinderWindowApplication
//
//  Created by infinit on 11/8/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAStatusBarController.h"
#import "IAStatusItemView.h"

@implementation IAStatusBarController
{
@private
    NSStatusItem* _status_item;
    IAStatusItemView* _status_item_view;
    NSImage* _status_icon;
    NSImage* _default_status_icon;
}

- (id)init
{
    self = [super init];
    if (self)
    {
        NSStatusBar* main_status_bar = [NSStatusBar systemStatusBar];
        _status_item = [main_status_bar statusItemWithLength:NSVariableStatusItemLength];

        _default_status_icon = [NSImage imageNamed:@"active_status_icon"];
        [_default_status_icon setTemplate:YES];
        [_status_item setImage:_default_status_icon];
        [_status_item setHighlightMode:YES];
        
        _status_item_view = [[IAStatusItemView alloc] initWithStatusItem:_status_item
                                                               withImage:[_status_item image]
                                                       andAlternateImage:[_status_item alternateImage]];

    }
    return self;
}

- (void)setIconHighlight:(bool)highlight
{
    [_status_item setHighlightMode:highlight];
}

- (IAStatusItemView*)getStatusItemView
{
    return _status_item_view;
}

@end
