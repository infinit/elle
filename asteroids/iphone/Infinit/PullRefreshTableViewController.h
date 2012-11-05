//
//  PullRefreshTableViewController.h
//  Plancast
//
//  Created by Leah Culver on 7/2/10.
//  Copyright (c) 2010 Leah Culver
//
//  Permission is hereby granted, free of charge, to any person
//  obtaining a copy of this software and associated documentation
//  files (the "Software"), to deal in the Software without
//  restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following
//  conditions:
//
//  The above copyright notice and this permission notice shall be
//  included in all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//  OTHER DEALINGS IN THE SOFTWARE.
//

#import <UIKit/UIKit.h>


@interface PullRefreshTableViewController : UITableViewController {
    UIView *refreshHeaderView;
    UILabel *refreshHeaderLabel;
    UIImageView *refreshHeaderArrow;
    UIActivityIndicatorView *refreshHeaderSpinner;
    BOOL isHeaderDragging;
    BOOL isHeaderLoading;
    NSString *textHeaderPull;
    NSString *textHeaderRelease;
    NSString *textHeaderLoading;

    UIView *refreshFooterView;
    UILabel *refreshFooterLabel;
    UIImageView *refreshFooterArrow;
    UIActivityIndicatorView *refreshFooterSpinner;
    BOOL isFooterDragging;
    BOOL isFooterLoading;
    NSString *textFooterPull;
    NSString *textFooterRelease;
    NSString *textFooterLoading;

    BOOL    isFooterPresent;
}

@property (nonatomic, retain) UIView *refreshHeaderView;
@property (nonatomic, retain) UILabel *refreshHeaderLabel;
@property (nonatomic, retain) UIImageView *refreshHeaderArrow;
@property (nonatomic, retain) UIActivityIndicatorView *refreshHeaderSpinner;
@property (nonatomic, copy) NSString *textHeaderPull;
@property (nonatomic, copy) NSString *textHeaderRelease;
@property (nonatomic, copy) NSString *textHeaderLoading;

@property (nonatomic, retain) UIView *refreshFooterView;
@property (nonatomic, retain) UILabel *refreshFooterLabel;
@property (nonatomic, retain) UIImageView *refreshFooterArrow;
@property (nonatomic, retain) UIActivityIndicatorView *refreshFooterSpinner;
@property (nonatomic, copy) NSString *textFooterPull;
@property (nonatomic, copy) NSString *textFooterRelease;
@property (nonatomic, copy) NSString *textFooterLoading;

@property BOOL                      notifIsRevealed;

- (void)setupStrings;
- (void)addPullToRefreshHeader;
- (void)startHeaderLoading;
- (void)stopHeaderLoading;
- (void)refreshHeader;

- (void)addPullToRefreshFooter;

@end
