//
//  FIIconOverlay.m
//  HelperTools
//
//  Created by Charles Guillot on 27/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOIconOverlay.h"
#import "OOTableCellDictKey.h"
#import "OOListCellDictKey.h"
#import "OOIconCellDictKey.h"
#import <Quartz/Quartz.h>

@implementation OOIconOverlay

@synthesize nodeStatusOperationQueue;
@synthesize syncingIconRef;
@synthesize syncedIconRef;
@synthesize disconnectedIconRef;

@synthesize syncingImage;
@synthesize syncedImage;
@synthesize disconnectedImage;

@synthesize nodesStatusDict;

+ (void) initialize
{
    [NSClassFromString(@"TListViewIconAndTextCell") 
     fi_swizzleMethod:@selector(drawIconWithFrame:) 
     withMethod:@selector(drawOverlayIconWithFrame:) 
     error:NULL];
    
    [NSClassFromString(@"TBrowserTableColumnViewController") 
     fi_swizzleMethod:@selector(tableView:willDisplayCell:forTableColumn:row:) 
     withMethod:@selector(tableViewWithNodeCaptcher:willDisplayCell:forTableColumn:row:) 
     error:NULL];
    
    [NSClassFromString(@"IKImageBrowserCell") 
     fi_swizzleMethod:@selector(layerForType:) 
     withMethod:@selector(OverlayLayerForType:) 
     error:NULL];
}

- (id) init
{
    self = [super init];
    
    if (!nodeStatusOperationQueue) {
        nodeStatusOperationQueue = [[NSOperationQueue alloc] init];
        [nodeStatusOperationQueue setMaxConcurrentOperationCount:2];
    }
    if(self) {
        NSString* imageName;
        
        imageName = [[NSBundle bundleWithIdentifier:@"io.infinit.FinderDopant"] pathForResource:@"ok" ofType:@"icns"];
        syncingImage = [[NSImage alloc] initWithContentsOfFile:imageName];
        syncingIconRef = [self iconRefForURL:[NSURL fileURLWithPath:imageName]];
        
        imageName = [[NSBundle bundleWithIdentifier:@"io.infinit.FinderDopant"] pathForResource:@"ok" ofType:@"icns"];
        syncedImage = [[NSImage alloc] initWithContentsOfFile:imageName];
        syncedIconRef = [self iconRefForURL:[NSURL fileURLWithPath:imageName]];
        
        imageName = [[NSBundle bundleWithIdentifier:@"io.infinit.FinderDopant"] pathForResource:@"ok" ofType:@"icns"];
        disconnectedImage = [[NSImage alloc] initWithContentsOfFile:imageName];
        disconnectedIconRef = [self iconRefForURL:[NSURL fileURLWithPath:imageName]];
	}
    return self;
}

+ (OOIconOverlay*) instance
{
	static OOIconOverlay *item = nil;
	
	if(item == nil)
		item = [[OOIconOverlay alloc] init];
	
	return item;
}

- (IconRef) iconRefForURL:(NSURL *)arg1
{
    FSRef   theFSRef;
    IconRef theIconRef;
    
    if( CFURLGetFSRef((CFURLRef)arg1, &theFSRef))
    {
        RegisterIconRefFromFSRef ('ANYT', 'HING', &theFSRef, &theIconRef );
    }
    
    return theIconRef;
}  
- (void) addStatusOpperationToQueue:(id)arg1
{
    if (arg1 != nil)
    {
        [nodeStatusOperationQueue addOperation:arg1];
    }
}

- (IconRef) iconRefWithNodeStatus:(OONodeStatus)arg1
{
    switch (arg1) {
        case OONodeStatusSynced:
            return [self syncedIconRef];
            break;
        case OONodeStatusSyncing:
            return [self syncingIconRef];
            break;
        case OONodeStatusDisconected:
            return [self disconnectedIconRef];
            break;
        default:
            return NULL;
            break;
    }
}

- (NSImage *) imageWithNodeStatus:(OONodeStatus)arg1
{
    switch (arg1) {
        case OONodeStatusSynced:
            return [self syncedImage];
            break;
        case OONodeStatusSyncing:
            return [self syncingImage];
            break;
        case OONodeStatusDisconected:
            return [self disconnectedImage];
            break;
        default:
            return nil;
            break;
    }
}

@end

@implementation NSTextFieldCell (FIIconOverlayBadge)

- (void) setOverlayIcon:(IconRef)arg1
{
    // Declare
    IconRef *badgedIconRef;
    TIconRef *iconRef;
    
    // Get icon
    iconRef = (TIconRef *)[self icon];
    IconRef backgroundIconRef = iconRef->fRef;
    // Get overley icon
    IconRef foregroundIconRef = arg1;
    
    CompositeIconRef (  backgroundIconRef,
                      foregroundIconRef,
                      &badgedIconRef);
    // Set adhoc struct
    const struct TFENode *tfeIcon = {badgedIconRef};
    // Send message to self seIcon method.
    objc_msgSend(self, @selector(setIcon:), &tfeIcon);
    
}

- (void) drawOverlayIconWithFrame:(struct CGRect)arg1
{
    // If self is a list cell.
    if (![self isKindOfClass:NSClassFromString(@"TColumnCell")])
    {
        // Check path => if it is an infinit path add icon overlay.
        if ([self respondsToSelector:@selector(node)]) {
            // TODO
            OOListCellDictKey *dictKey = [OOListCellDictKey listCellDictKeyWithCell:self];
            
            IconRef cellStatus = [[OOIconOverlay instance] iconRefWithNodeStatus:dictKey.nodeStatus];
            if (cellStatus != NULL) {
                [self setOverlayIcon:cellStatus];
            }
        }
    }
    // Default method
    [self drawOverlayIconWithFrame:arg1];
}

@end

@implementation NSViewController (FIIconOverlayBadge)

- (void) tableViewWithNodeCaptcher:(id)arg1 willDisplayCell:(id)arg2 forTableColumn:(id)arg3 row:(int)arg4;
{
    // Default method
    [self tableViewWithNodeCaptcher:arg1 willDisplayCell:arg2 forTableColumn:arg3 row:arg4];
    // If the cell is a TColumnCell
    if ([arg2 isKindOfClass:NSClassFromString(@"TColumnCell")]) {
        if ([arg2 respondsToSelector:@selector(node)]) {
            // If the path is an Infinit path.
            // TODO
            
            OOTableCellDictKey *dictKey = [OOTableCellDictKey tableCellDictKeyWithColumnIdentifer:arg3 rowIndex:arg4 forNode:[arg2 node]];
            // Check if a cell status has been retrieve.
            
            IconRef cellStatus = [[OOIconOverlay instance] iconRefWithNodeStatus:dictKey.nodeStatus];
            
            if (cellStatus != NULL) {
                // If yes draw icon
                [arg2 setOverlayIcon:cellStatus];
            }
        }
    }
}

@end

@implementation IKImageBrowserCell (OOIconOverlayBadge)

- (CALayer *) OverlayLayerForType:(NSString *) type
{
    if(type == IKImageBrowserCellForegroundLayer){
        
        // TODO
        OOIconCellDictKey *dictKey = [OOIconCellDictKey iconCellDictKeyWithCell:self];
        
        NSImage *cellStatus = [[OOIconOverlay instance] imageWithNodeStatus:dictKey.nodeStatus];
        if (cellStatus != nil) {
            
            CALayer *layer = [self FT_layerForType:type];
            NSRect frame = [(IKImageBrowserCell *)self frame];
            
            //create a selection layer
            CALayer *iconLayer = [CALayer layer];
            NSRect imgFrame = [(IKImageBrowserCell *)self imageFrame];
            iconLayer.frame = CGRectMake(imgFrame.origin.x - frame.origin.x, imgFrame.origin.y - frame.origin.y, imgFrame.size.width, imgFrame.size.height);
            
            if (layer == nil) {
                layer = [CALayer layer];
            }
            
            [layer setNeedsDisplay];
            layer.name = @"infinitOverlayLayer";
            //layer.minificationFilter = kCAFilterLinear;
            
            [layer addSublayer:iconLayer];
            iconLayer.contents = cellStatus;
            //iconLayer.minificationFilter = kCAFilterLinear;
            //iconLayer.contentsGravity = kCAGravityResizeAspect;
            
            return layer;
            
        }
    }
    return [self OverlayLayerForType:type];
    
}
@end


