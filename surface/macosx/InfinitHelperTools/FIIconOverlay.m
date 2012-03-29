//
//  FIIconOverlay.m
//  HelperTools
//
//  Created by Charles Guillot on 27/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FIIconOverlay.h"
#import "FITableCellDictKey.h"

@implementation FIIconOverlay

@synthesize syncingIconRef;
@synthesize syncedIconRef;
@synthesize disconnectedIconRef;

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
    
    [NSClassFromString(@"FINode") 
     fi_swizzleMethod:@selector(IKImageRepresentationWithType:) 
     withMethod:@selector(IKImageRepresentationWithTypeWithNodeCaptcher:) 
     error:NULL];
}

- (id) init
{
    self = [super init];
    
    if (!nodeStatusOperationQueue) {
        nodeStatusOperationQueue = [[NSOperationQueue alloc] init];
        [nodeStatusOperationQueue setMaxConcurrentOperationCount:2];
    }
    if (!nodesStatusDict) {
        nodesStatusDict = [[NSMutableDictionary alloc] init];
    }
    if(self) {
        NSString* imageName;
        
        imageName = [[NSBundle bundleWithPath:@"/Applications/Infinit.app"] pathForResource:@"ok" ofType:@"icns"];
        syncingIconRef = [self iconRefForURL:[NSURL fileURLWithPath:imageName]];
        
        imageName = [[NSBundle bundleWithPath:@"/Applications/Infinit.app"] pathForResource:@"ok" ofType:@"icns"];
        syncedIconRef = [self iconRefForURL:[NSURL fileURLWithPath:imageName]];
        
        imageName = [[NSBundle bundleWithPath:@"/Applications/Infinit.app"] pathForResource:@"ok" ofType:@"icns"];
        disconnectedIconRef = [self iconRefForURL:[NSURL fileURLWithPath:imageName]];
	}
    return self;
}

+ (FIIconOverlay*) instance
{
	static FIIconOverlay *item = nil;
	
	if(item == nil)
		item = [[FIIconOverlay alloc] init];
	
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
- (void) addCellStatusWithCell:(id)arg1
{
    if (arg1 != nil && [nodesStatusDict objectForKey:arg1] == nil )
    {
        [nodeStatusOperationQueue addOperationWithBlock:^{
            // Sleep to test... TODELETE
            sleep(1);
            
            if ([arg1 isKindOfClass:NSClassFromString(@"TListViewIconAndTextCell")])
            {
                if ([arg1 respondsToSelector:@selector(node)]) {
                    TFENode *node = [arg1 node];
                    // TODO get node status
                    
                    // Set new node status
                    CFDictionarySetValue( (CFMutableDictionaryRef)nodesStatusDict, arg1, [NSNumber numberWithInt:FISynced]);
                    
                    // Redraw cell
                    [[arg1 controlView] updateCell:arg1];
                    
                    // If is active schedule status getter. 
                }
            }
            else if ([arg1 isKindOfClass:[FITableCellDictKey class]])
            {
                
            }
        }];
        
        
        //CFDictionarySetValue((CFMutableDictionaryRef)nodeStatus,tIconAndTextCell, watingStatusImg);
        // Add task to operation queue
        
    }
}

- (void) removeCellStatusWithCell:(id)arg1
{
    [[self CellStatusWithCell:arg1] release];
	[nodesStatusDict removeObjectForKey:arg1];
}

- (id) CellStatusWithCell:(id)arg1
{
	return [nodesStatusDict objectForKey:arg1];
}

- (IconRef) iconRefWithCell:(id)arg1
{
    NSNumber *cellStatusNumber = [self CellStatusWithCell:arg1];
    if (cellStatusNumber == nil) {
        return NULL;
    }
    switch ([cellStatusNumber intValue]) {
        case FISynced:
            return [self syncedIconRef];
            break;
        case FISyncing:
            return [self syncingIconRef];
            break;
        default:
            return [self disconnectedIconRef];
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
            NSURL *path = [NSURL fileURLWithPath:
                           [[NSClassFromString(@"NSNavFBENode") _nodeWithFBENode:
                             ((TFENode *)[self node])->fNodeRef] path]];
            // TODO
            IconRef cellStatus = [[FIIconOverlay instance] iconRefWithCell:self];
            if (cellStatus == NULL)
            {
                // If no cell status has been set.
                [[FIIconOverlay instance] addCellStatusWithCell:self];
            }
            else
            {
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
    // if the cell is a TColumnCell
    if ([arg2 isKindOfClass:NSClassFromString(@"TColumnCell")])
    {
        // if the path is an Infinit path.
        // TODO
        
        id dictKey = [FITableCellDictKey tableCellDictKeyWithColumnIdentifer:arg3 rowIndex:arg4];
        // checl if a cell status has been retrieve.
        IconRef cellStatus = [[FIIconOverlay instance] iconRefWithCell:dictKey];
    
        if (cellStatus == NULL) {
            // If not get it.
            [[FIIconOverlay instance] addCellStatusWithCell:dictKey];
        }
        else
        {
            // If yes draw icon
            [arg2 setOverlayIcon:cellStatus];
        }
    }
    
    // Default method
    [self tableViewWithNodeCaptcher:arg1 willDisplayCell:arg2 forTableColumn:arg3 row:arg4];
}

@end

@implementation NSObject (FIIconOverlayBadge)

- (id)IKImageRepresentationWithTypeWithNodeCaptcher:(id)arg1
{
    if ([self isInIconView]) {
        //TODO
    }
    return [self IKImageRepresentationWithTypeWithNodeCaptcher:arg1];
}

@end