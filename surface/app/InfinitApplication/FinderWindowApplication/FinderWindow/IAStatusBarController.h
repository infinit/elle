//
//  IAStatusBarController.h
//  FinderWindowApplication
//
//  Created by infinit on 11/8/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "IAStatusItemView.h"

@interface IAStatusBarController : NSObject

- (IAStatusItemView*)getStatusItemView;
- (void)setIconHighlight:(bool)highlight;

@end
