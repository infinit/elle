//
//  DriveInfoViewController.h
//  Infinit
//
//  Created by Simon Peccaud on 9/4/12.
//  Copyright (c) 2012 infinit.io. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "InfinitNetworks.h"

/*@interface DriveInfoButton : UIButton

- (id) initWithTag:(int) tag cellFrame:(CGRect) cellFrame;
- (void)setHighlighted:(BOOL)highlighted animated:(BOOL)animated;
- (void)setSelected:(BOOL)selected animated:(BOOL)animated;

@end*/

@interface DriveCell : UITableViewCell

@property UIButton* infoButton;
@property UIImageView* offlineIcon;

- (void)setHighlighted:(BOOL)highlighted animated:(BOOL)animated;
- (void) hideInfoButton;
- (void) revealInfoButton;

//- (void)setSelected:(BOOL)selected animated:(BOOL)animated;

@end

@interface DriveInfoViewController : UITableViewController

@property (strong, nonatomic)   NSMutableArray  *infoList;
@property (strong) IBOutlet     UITableView     *oTableView;
@property                       InfinitNetwork  *ifNetwork;

- (DriveInfoViewController*) initWithInfinitNetwork:(InfinitNetwork*) ifNet;

@end
