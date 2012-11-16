//
//  IAStatusItemView.h
//  FinderWindowApplication
//
//  Created by infinit on 11/8/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface IAStatusItemView : NSView

@property (nonatomic, readonly) NSRect absoluteFrame;
@property (nonatomic, readonly) CGFloat width;
@property (nonatomic, readonly) CGFloat height;

- (id)initWithStatusItem:(NSStatusItem*)status_item
               withImage:(NSImage*)image
       andAlternateImage:(NSImage*)alternate_image;

@end
