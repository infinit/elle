#import "OOUserBrowserView.h"
#import "OOUserBrowserCell.h"

@implementation OOUserBrowserView

- (IKImageBrowserCell *)newCellForRepresentedItem:(id)cell {
	return [[OOUserBrowserCell alloc] init];
}

- (void)drawRect:(NSRect)dirtyRect
{
    //retrieve the visible area
	NSRect visibleRect = [self visibleRect];
	
	//compare with the visible rect at the previous frame
	if(!NSEqualRects(visibleRect, lastVisibleRect)){
		//we did scroll or resize, redraw the background
		[[self backgroundLayer] setNeedsDisplay];
		
		//update last visible rect
		lastVisibleRect = visibleRect;
	}
	
	[super drawRect:dirtyRect];
}

@end
