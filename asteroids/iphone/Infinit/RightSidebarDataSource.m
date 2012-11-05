//
//  RightSidebarDataSource.m
//  Infinitest
//
//  Created by Simon Peccaud on 5/31/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "RightSidebarDataSource.h"

@interface RightSidebarDataSource () {}     // Class extension with private properties
    @property(strong, nonatomic) NSArray*    _buttons;
@end

@implementation RightSidebarDataSource

@synthesize _buttons;

- (id) initWithTableViewController:(UITableViewController*) tableViewController
{
    self = [super init];

    // Add folder
    UIButton* folderButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [folderButton setImage:[UIImage imageNamed:@"folder-add.png"] forState:UIControlStateNormal];
    [folderButton addTarget:tableViewController action:@selector(oPresentFolderTextInput:) forControlEvents:UIControlEventTouchUpInside];
    folderButton.frame = CGRectMake(10, 10, 60, 60);

    // Take and add one picture
    UIButton* singleShotButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [singleShotButton setImage:[UIImage imageNamed:@"camera-single-add.png"] forState:UIControlStateNormal];
    [singleShotButton addTarget:tableViewController action:@selector(oPresentSingleShotView:) forControlEvents:UIControlEventTouchUpInside];
    singleShotButton.frame = CGRectMake(10, 5, 60, 60);

    // Choose picture from camera roll
    UIButton* photoAlbumButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [photoAlbumButton setImage:[UIImage imageNamed:@"camera-roll-add.png"] forState:UIControlStateNormal];
    [photoAlbumButton addTarget:tableViewController action:@selector(oPresentPhotoLibrary:) forControlEvents:UIControlEventTouchUpInside];
    photoAlbumButton.frame = CGRectMake(10, 10, 60, 60);

    // Choose picture from camera roll
    UIButton* linkButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [linkButton setImage:[UIImage imageNamed:@"link-add.png"] forState:UIControlStateNormal];
    [linkButton addTarget:tableViewController action:@selector(oPresentLinkTextInput:) forControlEvents:UIControlEventTouchUpInside];
    linkButton.frame = CGRectMake(10, 10, 60, 60);

    self._buttons = [[NSArray alloc] initWithObjects:folderButton, singleShotButton, photoAlbumButton, linkButton, nil];

    return self;
}

- (NSInteger) numberOfRowsInTableView:(UITableView *)tableView {
    return [_buttons count];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [_buttons count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *SimpleTableIdentifier = @"RighstSideBarCellIdentifier";

    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:SimpleTableIdentifier];
    NSUInteger row = [indexPath row];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:SimpleTableIdentifier];
    }
    UIButton* button = [self._buttons objectAtIndex:row];
    [cell addSubview:button];

    return cell;
}


@end
