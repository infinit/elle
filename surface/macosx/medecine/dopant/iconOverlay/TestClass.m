//
//  TestClass.m
//  HelperTools
//
//  Created by Charles Guillot - Infinit on 3/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "TestClass.h"
#import "CDStructures.h"
#import <Quartz/Quartz.h>

@implementation TestClass
+(void)initialize
{
    [NSClassFromString(@"IKImageBrowserCell") 
     fi_swizzleMethod:@selector(layerForType:) 
     withMethod:@selector(FT_layerForType:) 
     error:NULL];
    
    [NSClassFromString(@"IKImageBrowserCell") 
     fi_swizzleMethod:@selector(representedItem) 
     withMethod:@selector(representedItem2) 
     error:NULL];
}

- (id) init
{
    self = [super init];
    
    if(self) {
		// Init IVars
	}
    return self;
}
+ (TestClass*) instance
{
	static TestClass *item = nil;
	
	if(item == nil)
		item = [[TestClass alloc] init];
	
	return item;
}
@end

@implementation NSObject (TestClass)

- (CALayer *) FT_layerForType:(NSString *) type
{
    if(type == IKImageBrowserCellForegroundLayer){
        
        CALayer *layer = [self FT_layerForType:type];
        NSRect frame = [(IKImageBrowserCell *)self frame];
        NSImage *testImage = [[NSImage alloc] initWithContentsOfFile:[[NSBundle bundleWithPath:@"/Applications/Infinit.app"] pathForResource:@"ok" ofType:@"icns"]];
        //create a selection layer
        CALayer *iconLayer = [CALayer layer];
        NSRect imgFrame = [(IKImageBrowserCell *)self imageFrame];
        iconLayer.frame = CGRectMake(imgFrame.origin.x - frame.origin.x, imgFrame.origin.y - frame.origin.y, imgFrame.size.width, imgFrame.size.height);
        
        if (layer == nil) {
            layer = [CALayer layer];
        }
        
        [layer setNeedsDisplay];
        layer.name = @"infinitBadgingLayer";
        //layer.minificationFilter = kCAFilterLinear;
        
        
        
        [layer addSublayer:iconLayer];
        iconLayer.contents = testImage;
        //iconLayer.minificationFilter = kCAFilterLinear;
        //iconLayer.contentsGravity = kCAGravityResizeAspect;
        
        return layer;
    }
    else {
        return [self FT_layerForType:type];
    }

}
- (id)representedItem2
{
    id aa = [self representedItem2];
    // get Overlay Icon by path
    // if exist call layer for type
    // else ask to communicator
    return aa;
}
@end
