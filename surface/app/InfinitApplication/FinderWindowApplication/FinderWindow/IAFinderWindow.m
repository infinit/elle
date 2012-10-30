//
//  IAFinderWindow.m
//  FinderWindowApplication
//
//  Created by infinit on 10/26/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAFinderWindow.h"

@implementation IAFinderWindow

/**
 * A borderless window is not a "key window" by default, which implies
 * that contained fields are not editable. This override fix that behavior.
 */
- (BOOL)canBecomeKeyWindow
{
    return YES;
}

@end
