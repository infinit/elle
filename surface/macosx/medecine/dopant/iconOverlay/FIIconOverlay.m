//
//  FIIconOverlay.m
//  HelperTools
//
//  Created by Charles Guillot on 27/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FIIconOverlay.h"
#import "FITableCellDictKey.h"
#import "FIListCellDictKey.h"

@implementation FIIconOverlay

@synthesize nodeStatusOperationQueue;
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
    [NSClassFromString(@"NSTableColumn") 
     fi_swizzleMethod:@selector(dealloc) 
     withMethod:@selector(dealloc2) 
     error:NULL];
    
    [NSClassFromString(@"TBrowserTableColumnViewController") 
     fi_swizzleMethod:@selector(tableView:willDisplayCell:forTableColumn:row:) 
     withMethod:@selector(tableViewWithNodeCaptcher:willDisplayCell:forTableColumn:row:) 
     error:NULL];
    
    [NSClassFromString(@"FINode") 
     fi_swizzleMethod:@selector(IKImageRepresentationWithType:) 
     withMethod:@selector(IKImageRepresentationWithTypeWithNodeCaptcher:) 
     error:NULL];
    
    [NSClassFromString(@"TBrowserTableView") 
     fi_swizzleMethod:@selector(preparedCellAtColumn:row:) 
     withMethod:@selector(preparedCellAtColumn2:row:) 
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
        syncingIconRef = [self iconRefForURL:[NSURL fileURLWithPath:imageName]];
        
        imageName = [[NSBundle bundleWithIdentifier:@"io.infinit.FinderDopant"] pathForResource:@"ok" ofType:@"icns"];
        syncedIconRef = [self iconRefForURL:[NSURL fileURLWithPath:imageName]];
        
        imageName = [[NSBundle bundleWithIdentifier:@"io.infinit.FinderDopant"] pathForResource:@"ok" ofType:@"icns"];
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
        [nodeStatusOperationQueue addOperation:arg1];
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
            
            FITableCellDictKey *dictKey = [FITableCellDictKey tableCellDictKeyWithColumnIdentifer:arg3 rowIndex:arg4 forNode:[arg2 node]];
            // Check if a cell status has been retrieve.
            
            IconRef cellStatus = [[FIIconOverlay instance] iconRefWithCell:dictKey.nodeStatus];
            
            if (cellStatus != NULL) {
                // If yes draw icon
                [arg2 setOverlayIcon:cellStatus];
            }
        }
    }
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

@implementation NSTableView (FIIconOverlayBadge)

- (id)preparedCellAtColumn2:(int)arg1 row:(int)arg2;
{
    id returnValue = [self preparedCellAtColumn2:arg1 row:arg2];
    return returnValue;
}

@end

@implementation NSTableColumn (FIIconOverlayBadge)

- (void) dealloc2
{
    [self dealloc2];
}

@end

