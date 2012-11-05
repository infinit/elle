//
//  InfinitestCell.m
//  Infinitest
//
//  Created by Simon Peccaud on 3/22/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "InfinitestCell.h"

@implementation InfinitestCell

@synthesize label;
@synthesize iconView;

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier {
    if (self = [super initWithFrame:frame reuseIdentifier:reuseIdentifier]) {
        // Initialization code
        label = [[UILabel alloc]init];
        label.textAlignment = UITextAlignmentLeft;
        label.font = [UIFont systemFontOfSize:14];
        iconView = [[UIImageView alloc]init];
        [self.contentView addSubview:label];
        [self.contentView addSubview:iconView];
        
    }
    return self;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    CGRect contentRect = self.contentView.bounds;
    CGFloat boundsX = contentRect.origin.x;
    CGRect frame;
    frame= CGRectMake(boundsX+10 ,0, 50, 50);
    iconView.frame = frame;
    
    frame= CGRectMake(boundsX+70 ,5, 200, 25);
    label.frame = frame;
}

@end
