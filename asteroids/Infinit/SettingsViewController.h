//
//  SettingsViewController.h
//  Infinitest
//
//  Created by Simon Peccaud on 5/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SettingsViewController : UITableViewController

@property(strong, nonatomic) IBOutlet UITableView*   oTableView;
@property(strong, nonatomic) NSMutableArray* optionsList;

- (IBAction)logOut:(id)sender;

@end
