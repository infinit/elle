//
//  ITSearchWindow.h
//  search
//
//  Created by infinit on 12/5/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ITSearchWindow : NSWindow

- (id)initWithParent:(NSWindow*)parent;

- (void)updatePosition:(NSRect)pos;

- (void)hide;
- (void)show;

@property (nonatomic, readonly) BOOL hidden;

@end
