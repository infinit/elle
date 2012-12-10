//
//  IASwaggersView.m
//  FinderWindow
//
//  Created by Patrick Perlmutter on 11/28/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IASwaggersView.h"

#define CIRCLE_OFFSET           140     /* pixels */
#define CIRCLE_INITIAL_OFFSET   60      /* pixels */
#define CIRCLE_LINE_WIDTH       3.0f    /* pixels */
#define CIRCLE_HEIGHT_RATIO     0.8f
#define CIRCLE_VERTICAL_OFFSET  50      /* pixels */
#define SELF_RADIUS             30      /* pixels */

#define COLOR(c)            [NSColor c ## Color]
#define BACKGROUND_COLOR    COLOR(green)
#define CIRCLE_COLOR        COLOR(black)

@implementation IASwaggersView
{
@private
    NSBezierPath*   _self;
    NSImage*        _self_image;
    NSMutableArray* _circles;
    unsigned int    _circle_count;
    NSMutableArray* _swaggers;
}

- (id)initWithFrame:(NSRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        _self = [NSBezierPath bezierPath];
        NSString* path = [[NSBundle bundleWithIdentifier:@"io.infinit.FinderWindow"] pathForImageResource:@"Kitten"];
        _self_image = [[NSImage alloc] initWithContentsOfFile:path];
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    NSRect bounds = [self bounds];
    [BACKGROUND_COLOR set];
    [NSBezierPath fillRect:bounds];
    
    [CIRCLE_COLOR set];
    for (int i = 0; i < _circle_count; ++i)
        [[_circles objectAtIndex:i] stroke];
    
//    [BACKGROUND_COLOR set];
//    [NSBezierPath fillRect:[_self bounds]];

    [NSGraphicsContext saveGraphicsState];
    [_self setClip];
    [_self_image drawInRect:[_self bounds]
                   fromRect:NSMakeRect(0, 0, _self_image.size.width, _self_image.size.height)
                  operation:NSCompositeCopy
                   fraction:1];
    [NSGraphicsContext restoreGraphicsState];
    
    for (int i = 0; i < [_swaggers count]; ++i)
    {
        [[_swaggers objectAtIndex:i] stroke];
    }
//    [COLOR(clear) set];
//    [_self fill];
}

- (void)setFrame:(NSRect)frameRect
{
    [self refreshWithFrame:frameRect];
    [super setFrame:frameRect];
}

- (void)refreshWithFrame:(NSRect)frame
{
    [self _computeCircleCountForFrame:frame];
    NSPoint origin;
    origin.x = frame.origin.x + frame.size.width / 2;
    origin.y = frame.origin.y;
    NSMutableArray* circles = [NSMutableArray arrayWithCapacity:_circle_count];
    for (int i = 0; i < _circle_count; ++i)
    {
        float radius = (i + 1) * CIRCLE_OFFSET + CIRCLE_INITIAL_OFFSET;
        NSRect rect = NSMakeRect(origin.x - radius,
                                 origin.y - radius  * CIRCLE_HEIGHT_RATIO - CIRCLE_VERTICAL_OFFSET,
                                 radius * 2.0f,
                                 radius * CIRCLE_HEIGHT_RATIO * 2.0f);
        NSBezierPath* path = [NSBezierPath bezierPath];
        [path appendBezierPathWithOvalInRect:rect];
        [circles insertObject:path
                      atIndex:i];
    }
    _circles = circles;
    
    [_self removeAllPoints];
    NSPoint self_center = NSMakePoint(frame.size.width / 2, SELF_RADIUS + 10);
    [_self appendBezierPathWithArcWithCenter:self_center
                                      radius:SELF_RADIUS
                                  startAngle:0
                                    endAngle:360];
    
    int nb_swag = 300; //xXX
    NSMutableArray* swaggers_circles = [NSMutableArray arrayWithCapacity:nb_swag];
    int arc = 1;
    double alpha = 20.0;
    double alpha_incr = 15.0;
    for (int i = 0; i < nb_swag; ++i)
    {
        double radius = (arc) * CIRCLE_OFFSET + CIRCLE_INITIAL_OFFSET;
        double x = origin.x + cos(alpha * 3.14159265 / 180.0) * radius;
        double y = origin.y - CIRCLE_VERTICAL_OFFSET + sin(alpha * 3.14159265 / 180.0) * radius * CIRCLE_HEIGHT_RATIO;
        //NSLog(@"SWAG at %f %f", x, y);
        NSRect rect = NSMakeRect(x - SELF_RADIUS / 2.0, y - SELF_RADIUS / 2.0, SELF_RADIUS, SELF_RADIUS);
        NSBezierPath* path = [NSBezierPath bezierPath];
        [path appendBezierPathWithOvalInRect:rect];
        
        [swaggers_circles insertObject:path atIndex:i];
        
        alpha += alpha_incr;
        //NSLog(@"Swag %d, alpha = %f", i, alpha);
        
        if (alpha > (180 - alpha_incr))
        {
            alpha = 20;
            arc += 1;
        }
        
    }
    _swaggers = swaggers_circles;
}

- (void)_computeCircleCountForFrame:(NSRect)frame
{
    float radius;
    int i = 0;
    float max_radius = MIN(frame.size.width / 2.0f  - (CIRCLE_INITIAL_OFFSET),
                           (frame.size.height) * CIRCLE_HEIGHT_RATIO + CIRCLE_VERTICAL_OFFSET);
    while (radius < max_radius)
    {
        radius = (i + 1) * CIRCLE_OFFSET + CIRCLE_INITIAL_OFFSET;
        i += 1;
    }
    _circle_count = i;
}

@end
