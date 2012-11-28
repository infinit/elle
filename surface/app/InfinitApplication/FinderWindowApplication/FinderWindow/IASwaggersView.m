//
//  IASwaggersView.m
//  FinderWindow
//
//  Created by Patrick Perlmutter on 11/28/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IASwaggersView.h"

#define CIRCLE_COUNT            2
#define CIRCLE_OFFSET           140     /* pixels */
#define CIRCLE_INITIAL_OFFSET   60      /* pixels */
#define CIRCLE_LINE_WIDTH       3.0f    /* pixels */
#define CIRCLE_HEIGHT_RATIO     0.8f
#define CIRCLE_VERTICAL_OFFSET  50      /* pixels */

#define BACKGROUND_COLOR    greenColor
#define CIRCLE_COLOR        blackColor

@implementation IASwaggersView
{
@private
    NSArray* _circles;
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    
    NSPoint origin;
    origin.x = frame.origin.x + frame.size.width / 2;
    origin.y = frame.origin.y;
    
    if (self)
    {
        NSMutableArray* circles = [NSMutableArray arrayWithCapacity:CIRCLE_COUNT];
        for (int i = 0; i < CIRCLE_COUNT; ++i)
        {
            float radius = (i + 1) * CIRCLE_OFFSET + CIRCLE_INITIAL_OFFSET;
            NSRect rect = NSMakeRect(origin.x - radius,
                                     origin.y - radius  * CIRCLE_HEIGHT_RATIO - CIRCLE_VERTICAL_OFFSET,
                                     radius * 2.0f,
                                     radius * CIRCLE_HEIGHT_RATIO * 2.0f);
            NSBezierPath* path = [NSBezierPath bezierPath];
            [path appendBezierPathWithOvalInRect:rect];
            [circles insertObject:path atIndex:i];
        }
        _circles = [NSArray arrayWithArray:circles]; // we won't mutate the circles
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    NSRect bounds = [self bounds];
    [[NSColor BACKGROUND_COLOR] set];
    [NSBezierPath fillRect:bounds];
    
    [[NSColor CIRCLE_COLOR] set];
    for (int i = 0; i < CIRCLE_COUNT; ++i)
        [[_circles objectAtIndex:i] stroke];
}

@end
