//
//  IANotificationPanelController.m
//  FinderWindowApplication
//
//  Created by infinit on 11/9/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IANotificationPanelController.h"
#import "IANotificationPanel.h"

@implementation IANotificationPanelController
{
@private
    BOOL _visibility;
}

- (void)awakeFromNib
{
    NSLog(@"YEAH I'm there");
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
