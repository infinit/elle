//
//  InfinitestCell.h
//  Infinitest
//
//  Created by Simon Peccaud on 3/22/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface InfinitestCell : UITableViewCell {
    UILabel *label;
    UIImageView *iconView;
}
@property(nonatomic, strong)UILabel *label;
@property(nonatomic, strong)UIImageView *iconView;
@end
