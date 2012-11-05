//
//  TopNotifViewController.h
//  Infinit
//
//  Created by Simon Peccaud on 10/3/12.
//  Copyright (c) 2012 infinit.io. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TopNotifViewController : UIViewController

@property UINavigationController*   navigationCtrl;
@property UILabel*                  textLabel;
@property BOOL                      isRevealed;

- (id)initWithView:(UINavigationController*) aView;

@end
