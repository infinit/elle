//
//  IASearchResultsWindow.h
//  FinderWindow
//
//  Created by Patrick Perlmutter on 12/6/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface IASearchResultsWindow : NSWindow

- (id)initWithParent:(NSWindow*)parent;

- (void)updatePosition:(NSRect)pos;

- (void)hide;
- (void)show;

@property (nonatomic, readonly) BOOL hidden;

@end
