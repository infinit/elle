//
//  IHFDrawIconWithFrame.m
//  InfinitHelperTools
//
//  Created by Charles Guillot - Infinit on 3/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FIIconOverlayBadge.h"

static NSOperationQueue* fi_IconOverlayOperationQueue = nil;

@implementation FIIconOverlayBadge

@synthesize satusCheckedImgRef;

+ (void) initialize
{
    [NSClassFromString(@"TListViewIconAndTextCell") 
     fi_swizzleMethod:@selector(drawIconWithFrame:) 
     withMethod:@selector(drawOverlayIconWithFrame:) 
     error:NULL];
}

- (id) init
{
    self = [super init];
    
    if (!fi_IconOverlayOperationQueue) {
        fi_IconOverlayOperationQueue = [[NSOperationQueue alloc] init];
        [fi_IconOverlayOperationQueue setMaxConcurrentOperationCount:2];
    }
    if(self) {
        
        NSString* imageName = [[NSBundle bundleWithPath:@"/Applications/Infinit.app"] pathForResource:@"ok" ofType:@"icns"];
        NSURL *fileURL = [NSURL fileURLWithPath:imageName];

        satusCheckedImgRef = [self iconRefForURL:fileURL];
	}
    return self;
}
                               
- (IconRef)iconRefForURL:(NSURL *)aURL
{
    FSRef   theFSRef;
    IconRef theIconRef;
            
    if( CFURLGetFSRef((CFURLRef)aURL, &theFSRef))
    {
        RegisterIconRefFromFSRef ('ANYT', 'HING', &theFSRef, &theIconRef );
    }
            
    return theIconRef;
}                                             
+ (FIIconOverlayBadge*) instance
{
	static FIIconOverlayBadge *item = nil;
	
	if(item == nil)
		item = [[FIIconOverlayBadge alloc] init];
	
	return item;
}
- (void) addNodeObserver:(id)tIconAndTextCell
{
    if (tIconAndTextCell != nil && [nodeStatus objectForKey:tIconAndTextCell] == nil )
    {
        //CFDictionarySetValue((CFMutableDictionaryRef)nodeStatus,tIconAndTextCell, watingStatusImg);
        // Add task to operation queue
        [fi_IconOverlayOperationQueue addOperationWithBlock:^{
            sleep(1);
            //CFDictionarySetValue((CFMutableDictionaryRef)nodeStatus,tIconAndTextCell, satusChecked);
            //[self UpdateViewLayout];
            [[tIconAndTextCell controlView] updateCell:tIconAndTextCell];
        }];
    }
}

- (void) removeNodeObserver:(id)tIconAndTextCell
{
    [[self NodeObserver:tIconAndTextCell] release];
	[nodeStatus removeObjectForKey:tIconAndTextCell];
}

- (id) NodeObserver:(id)tIconAndTextCell
{
	return [nodeStatus objectForKey:tIconAndTextCell];
}
- (void) UpdateViewLayout
{
    [[[NSClassFromString(@"TGlobalWindowController") globalWindowController] frontmostBrowserWindowControllerIncludingDesktop] drawCompletelyIntoBackBuffer];
}
/*- (void) new_setImage
{
    //sleep(2);
    NSString* imageName = [[NSBundle bundleWithPath:@"/Applications/Infinit.app"] pathForResource:@"pastille" ofType:@"png"];
    NSImage *image = [[NSImage alloc] initWithContentsOfFile:imageName];
    //objc_setAssociatedObject(self, "myImage", image, OBJC_ASSOCIATION_COPY);
}*/
@end

@implementation NSTextFieldCell (FIIconOverlayBadge)


- (void) drawOverlayIconWithFrame:(struct CGRect)arg1
{
    IconRef *badgedIconRef;
    TIconRef *iconRef;
    iconRef = (TIconRef *)[self icon];
    IconRef backgroundIconRef = iconRef->fRef;
    IconRef foregroundIconRef = [[FIIconOverlayBadge instance] satusCheckedImgRef];
    CompositeIconRef (  backgroundIconRef,
                        foregroundIconRef,
                        &badgedIconRef);
    const struct TFENode *tfeIcon = {badgedIconRef};
    objc_msgSend(self, @selector(setIcon:), &tfeIcon);
    [self drawOverlayIconWithFrame:arg1];
    TFENode *node = [self node];
    if ([self respondsToSelector:@selector(node)]) {
        //TFENode *node = [self node];
        
        //NSImage *image = [[FIIconOverlayBadge instance] NodeObserver:self];
        //image = objc_getAssociatedObject(self, "myImage");
        void* ii = node->fNodeRef;
        NSString *aa = [[NSClassFromString(@"NSNavFBENode") _nodeWithFBENode:node->fNodeRef] path];
        /*if (image == nil)
        {
            [[FIIconOverlayBadge instance] addNodeObserver:self];
        return;
        }
        
        [image drawInRect:NSMakeRect(arg1.origin.x + 2*arg1.size.height/3, arg1.origin.y + 2*arg1.size.height/3, arg1.size.height/3, arg1.size.height/3) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];*/
    }
    else
    {
        
        [self drawOverlayIconWithFrame:arg1];
    }
}

@end
