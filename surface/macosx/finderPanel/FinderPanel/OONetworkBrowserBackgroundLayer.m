#import "OONetworkBrowserBackgroundLayer.h"
#import "OOBrowserUtilities.h"

static CGImageRef backgroundImage()
{
	static CGImageRef image = NULL;
	
	if(image == NULL)
		image = createImageWithName(@"handmadepaper.png");
	
	return image;
}


@implementation OONetworkBrowserBackgroundLayer

@synthesize owner;

// -------------------------------------------------------------------------
//	init
// -------------------------------------------------------------------------
- (id) init
{
	if((self = [super init])){
		//needs to redraw when bounds change
		[self setNeedsDisplayOnBoundsChange:YES];
	}
	
	return self;
}

// -------------------------------------------------------------------------
//	actionForKey:
//
// always return nil, to never animate
// -------------------------------------------------------------------------
- (id<CAAction>)actionForKey:(NSString *)event
{
	return nil;
}

// -------------------------------------------------------------------------
//	drawInContext:
//
// draw a metal background that scrolls when the image browser scroll
// -------------------------------------------------------------------------
- (void)drawInContext:(CGContextRef)context
{
	//retreive bounds and visible rect
	NSRect visibleRect = [owner visibleRect];
	NSRect bounds = [owner bounds];
	
	//retreive background image
	CGImageRef image = backgroundImage();
	float width = (float) CGImageGetWidth(image);
	float height = (float) CGImageGetHeight(image);
	
	//compute coordinates to fill the view
	float left, top, right, bottom;
	
	top = bounds.size.height - NSMaxY(visibleRect);
	top = fmod(top, height);
	top = height - top;
	
	right = NSMaxX(visibleRect);
	bottom = -height;
	
	// tile the image and take in account the offset to 'emulate' a scrolling background
	for (top = visibleRect.size.height-top; top>bottom; top -= height){
		for(left=0; left<right; left+=width){
			CGContextDrawImage(context, CGRectMake(left, top, width, height), image);
		}
	}
}

@end
