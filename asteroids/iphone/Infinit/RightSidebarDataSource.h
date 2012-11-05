//
//  RightSidebarDataSource.h
//  Infinitest
//
//  Created by Simon Peccaud on 5/31/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface RightSidebarDataSource : NSObject <UITableViewDataSource>

- (id) initWithTableViewController: (UITableViewController*) tableViewController;
- (NSInteger)                                   numberOfRowsInTableView:(UITableView *)tableView;
- (NSInteger)                                   tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section;
- (UITableViewCell*)                            tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath;

@end
