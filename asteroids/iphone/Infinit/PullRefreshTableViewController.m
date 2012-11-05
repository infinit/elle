//
//  PullRefreshTableViewController.m
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

#import <QuartzCore/QuartzCore.h>
#import "PullRefreshTableViewController.h"

#define REFRESH_HEIGHT 52.0f


@implementation PullRefreshTableViewController

@synthesize textHeaderPull, textHeaderRelease, textHeaderLoading, refreshHeaderView, refreshHeaderLabel, refreshHeaderArrow, refreshHeaderSpinner,
    textFooterPull, textFooterRelease, textFooterLoading, refreshFooterView,refreshFooterLabel, refreshFooterArrow, refreshFooterSpinner, notifIsRevealed;

- (id)initWithStyle:(UITableViewStyle)style {
  self = [super initWithStyle:style];
  if (self != nil) {
    [self setupStrings];
  }
  return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder {
  self = [super initWithCoder:aDecoder];
  if (self != nil) {
    [self setupStrings];
  }
  return self;
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
  self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
  if (self != nil) {
    [self setupStrings];
  }
  return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self addPullToRefreshHeader];
    [self addPullToRefreshFooter];

    [[NSNotificationCenter defaultCenter]   addObserver:self
                                               selector:@selector(revealedNotifBar:)
                                                   name:@"revealNotifBar"
                                                 object:nil];
    [[NSNotificationCenter defaultCenter]   addObserver:self
                                               selector:@selector(hiddenNotifBar:)
                                                   name:@"hideNotifBar"
                                                 object:nil];
}

- (void)setupStrings{
    textHeaderPull = @"Pull down to refresh...";
    textHeaderRelease = @"Release to refresh...";
    textHeaderLoading = @"Loading...";

    textFooterPull = @"Pull up to load more...";
    textFooterRelease = @"Release to load more...";
    textFooterLoading = @"Loading...";
}

- (void)addPullToRefreshHeader {
    refreshHeaderView = [[UIView alloc] initWithFrame:CGRectMake(0, 0 - REFRESH_HEIGHT, 320, REFRESH_HEIGHT)];
    refreshHeaderView.backgroundColor = [UIColor clearColor];

    refreshHeaderLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 320, REFRESH_HEIGHT)];
    refreshHeaderLabel.backgroundColor = [UIColor clearColor];
    refreshHeaderLabel.font = [UIFont boldSystemFontOfSize:12.0];
    refreshHeaderLabel.textAlignment = UITextAlignmentCenter;

    refreshHeaderArrow = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"infinit_refresh.png"]];
    refreshHeaderArrow.frame = CGRectMake(floorf((REFRESH_HEIGHT - 27) / 2),
                                    (floorf(REFRESH_HEIGHT - 44) / 2),
                                    27, 44);

    refreshHeaderSpinner = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
    refreshHeaderSpinner.frame = CGRectMake(floorf(floorf(REFRESH_HEIGHT - 20) / 2), floorf((REFRESH_HEIGHT - 20) / 2), 20, 20);
    refreshHeaderSpinner.hidesWhenStopped = YES;

    [refreshHeaderView addSubview:refreshHeaderLabel];
    [refreshHeaderView addSubview:refreshHeaderArrow];
    [refreshHeaderView addSubview:refreshHeaderSpinner];
    [self.tableView addSubview:refreshHeaderView];
}

- (void)addPullToRefreshFooter {
    refreshFooterView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 320, REFRESH_HEIGHT)];
    refreshFooterView.backgroundColor = [UIColor clearColor];

    refreshFooterLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 320, REFRESH_HEIGHT)];
    refreshFooterLabel.backgroundColor = [UIColor clearColor];
    refreshFooterLabel.font = [UIFont boldSystemFontOfSize:12.0];
    refreshFooterLabel.textAlignment = UITextAlignmentCenter;

    refreshFooterArrow = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"infinit_refresh.png"]];
    refreshFooterArrow.frame = CGRectMake(floorf((REFRESH_HEIGHT - 27) / 2),
                                          (floorf(REFRESH_HEIGHT - 44) / 2),
                                          27, 44);

    refreshFooterSpinner = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
    refreshFooterSpinner.frame = CGRectMake(floorf(floorf(REFRESH_HEIGHT - 20) / 2), floorf((REFRESH_HEIGHT - 20) / 2), 20, 20);
    refreshFooterSpinner.hidesWhenStopped = YES;

    [refreshFooterView addSubview:refreshFooterLabel];
    [refreshFooterView addSubview:refreshFooterArrow];
    [refreshFooterView addSubview:refreshFooterSpinner];

    self.tableView.tableFooterView = refreshFooterView;
    self->isFooterPresent = YES;
}

- (void)scrollViewWillBeginDragging:(UIScrollView *)scrollView {

    if (isHeaderLoading) return;
        isHeaderDragging = YES;
    if (isFooterPresent)
    {
        if (isFooterLoading) return;
        isFooterDragging = YES;
    }
}

- (void)scrollViewDidScroll:(UIScrollView *)scrollView {
    if (!isHeaderLoading && isHeaderDragging && scrollView.contentOffset.y < 0) {
        // Update the arrow direction and label
        [UIView beginAnimations:nil context:NULL];
        if (scrollView.contentOffset.y < -REFRESH_HEIGHT) {
            // User is scrolling above the header
            refreshHeaderLabel.text = self.textHeaderRelease;
            [refreshHeaderArrow layer].transform = CATransform3DMakeRotation(M_PI, 0, 0, 1);
        } else { // User is scrolling somewhere within the header
            refreshHeaderLabel.text = self.textHeaderPull;
            [refreshHeaderArrow layer].transform = CATransform3DMakeRotation(M_PI * 2, 0, 0, 1);
        }
        [UIView commitAnimations];
    }
    if (isFooterPresent && !isFooterLoading && isFooterDragging && scrollView.contentOffset.y > 0) {
        // Update the arrow direction and label
        [UIView beginAnimations:nil context:NULL];
        if (scrollView.contentOffset.y > [self.tableView rowHeight] * [self.tableView numberOfRowsInSection:0] / 2 - REFRESH_HEIGHT) {
            // User is scrolling behind the footer
            refreshFooterLabel.text = self.textFooterRelease;
            [refreshFooterArrow layer].transform = CATransform3DMakeRotation(M_PI, 0, 0, 1);
        } else { // User is scrolling somewhere within the footer
            refreshFooterLabel.text = self.textFooterPull;
            [refreshFooterArrow layer].transform = CATransform3DMakeRotation(M_PI * 2, 0, 0, 1);
        }
        [UIView commitAnimations];
    }
}

- (void)scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate {
    if (isHeaderLoading) return;
    isHeaderDragging = NO;
    if (scrollView.contentOffset.y <= -REFRESH_HEIGHT) {
        // Released above the header
        [self startHeaderLoading];
    }
    if (isFooterPresent && isFooterLoading) return;
    isFooterDragging = NO;
    if (scrollView.contentOffset.y <= [[self view] bounds].size.height-REFRESH_HEIGHT) {
        // Released behind the footer
        [self startFooterLoading];
    }
}

- (void)startHeaderLoading {
    isHeaderLoading = YES;

    // Show the header
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:0.3];
    self.tableView.contentInset = UIEdgeInsetsMake(REFRESH_HEIGHT, 0, 0, 0);
    refreshHeaderLabel.text = self.textHeaderLoading;
    refreshHeaderArrow.hidden = YES;
    [refreshHeaderSpinner startAnimating];
    [UIView commitAnimations];

    // Refresh action!
    [self refreshHeader];
}

- (void)stopHeaderLoading {
    isHeaderLoading = NO;

    // Hide the header
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDelegate:self];
    [UIView setAnimationDuration:0.3];
    [UIView setAnimationDidStopSelector:@selector(stopHeaderLoadingComplete:finished:context:)];
    //self.tableView.contentInset = UIEdgeInsetsZero;
    UIEdgeInsets tableContentInset = self.tableView.contentInset;
    if (self.notifIsRevealed)
        tableContentInset.top = 20.0;
    else
        tableContentInset.top = 0.0;

    self.tableView.contentInset = tableContentInset;
    [refreshHeaderArrow layer].transform = CATransform3DMakeRotation(M_PI * 2, 0, 0, 1);
    [UIView commitAnimations];
}

- (void)stopHeaderLoadingComplete:(NSString *)animationID finished:(NSNumber *)finished context:(void *)context {
    // Reset the header
    refreshHeaderLabel.text = self.textHeaderPull;
    refreshHeaderArrow.hidden = NO;
    [refreshHeaderSpinner stopAnimating];
}

- (void)refreshHeader {
    // This is just a demo. Override this method with your custom reload action.
    // Don't forget to call stopLoading at the end.
    [self performSelector:@selector(stopHeaderLoading) withObject:nil afterDelay:2.0];
}


- (void)startFooterLoading {
    if (isFooterPresent) {
        isFooterLoading = YES;

        // Show the footer
        [UIView beginAnimations:nil context:NULL];
        [UIView setAnimationDuration:0.3];
        self.tableView.contentInset = UIEdgeInsetsMake(REFRESH_HEIGHT, 0, 0, 0);
        refreshFooterLabel.text = self.textFooterLoading;
        refreshFooterArrow.hidden = YES;
        [refreshFooterSpinner startAnimating];
        [UIView commitAnimations];
        // Refresh action!
        [self refreshFooter];
    }
}

- (void)stopFooterLoading {
    if (isFooterPresent) {
        isFooterLoading = NO;
        // Hide the footer
        [UIView beginAnimations:nil context:NULL];
        [UIView setAnimationDelegate:self];
        [UIView setAnimationDuration:0.3];
        [UIView setAnimationDidStopSelector:@selector(stopFooterLoadingComplete:finished:context:)];
        self.tableView.contentInset = UIEdgeInsetsZero;
        UIEdgeInsets tableContentInset = self.tableView.contentInset;
        tableContentInset.top = 0.0;
        self.tableView.contentInset = tableContentInset;
        [refreshFooterArrow layer].transform = CATransform3DMakeRotation(M_PI * 2, 0, 0, 1);
        [UIView commitAnimations];
    }
}

- (void)stopFooterLoadingComplete:(NSString *)animationID finished:(NSNumber *)finished context:(void *)context {
    if (isFooterPresent) {
    // Reset the footer
    refreshFooterLabel.text = self.textFooterPull;
    refreshFooterArrow.hidden = NO;
    [refreshFooterSpinner stopAnimating];
    }
}

- (void)refreshFooter {

    // This is just a demo. Override this method with your custom reload action.
    // Don't forget to call stopLoading at the end.
    [self performSelector:@selector(stopFooterLoading) withObject:nil afterDelay:5.0];
}

- (void) revealedNotifBar:(NSNotification*) notification {
    self.notifIsRevealed = TRUE;
}

- (void) hiddenNotifBar: (NSNotification*) notification {
    self.notifIsRevealed = FALSE;
}

@end
