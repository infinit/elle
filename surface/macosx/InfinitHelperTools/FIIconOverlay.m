//
//  FIIconOverlay.m
//  HelperTools
//
//  Created by Charles Guillot on 27/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FIIconOverlay.h"
#import "FITableCellDictKey.h"
#import "FIGetStatusOperation.h"
#import "FIListCellDictKey.h"

@implementation FIIconOverlay

@synthesize syncingIconRef;
@synthesize syncedIconRef;
@synthesize disconnectedIconRef;

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
- (void) addStatusOpperationToQueue:(id)arg1
{
    if (arg1 != nil)
    {
        [nodeStatusOperationQueue addOperation:[[FIGetStatusOperation alloc] initWithDictKey:arg1]];
    }
}

- (IconRef) iconRefWithCell:(FINodeStatus)arg1
{
    switch (arg1) {
        case FINodeStatusSynced:
            return [self syncedIconRef];
            break;
        case FINodeStatusSyncing:
            return [self syncingIconRef];
            break;
        case FINodeStatusDisconected:
            return [self disconnectedIconRef];
            break;
        default:
            return NULL;
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
            FIListCellDictKey *dictKey = [FIListCellDictKey listCellDictKeyWithCell:self];
            
            IconRef cellStatus = [[FIIconOverlay instance] iconRefWithCell:dictKey.nodeStatus];
            if (cellStatus == NULL)
            {
                // If no cell status has been set.
                [[FIIconOverlay instance] addStatusOpperationToQueue:dictKey];
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
        if ([self respondsToSelector:@selector(node)]) {
            // if the path is an Infinit path.
            // TODO
            
            FITableCellDictKey *dictKey = [FITableCellDictKey tableCellDictKeyWithColumnIdentifer:arg3 rowIndex:arg4 forNode:[arg2 node]];
            // checl if a cell status has been retrieve.
            
            IconRef cellStatus = [[FIIconOverlay instance] iconRefWithCell:dictKey.nodeStatus];
            
            if (cellStatus == NULL) {
                // If not get it.
                [[FIIconOverlay instance] addStatusOpperationToQueue:dictKey];
            }
            else
            {
                // If yes draw icon
                [arg2 setOverlayIcon:cellStatus];
            }
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