//
//  ITSearchWindow.h
//  search
//
//  Created by infinit on 12/5/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ITSearchWindow : NSWindow

- (void)showWithPosition:(NSRect)pos;

- (void)updatePosition:(NSRect)pos;

- (void)hide;

@property (nonatomic, readonly) BOOL hidden;

@end
