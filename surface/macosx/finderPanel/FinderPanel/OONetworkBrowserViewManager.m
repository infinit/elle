#import "OONetworkBrowserViewManager.h"
#import "OONetworkBrowserBackgroundLayer.h"
#import "OONetworkModel.h"
#import "OOUserModel.h"
#import "OONetworkAddButton.h"
#import "OOUserBrowserView.h"
#import <Phone/OOPhone.h>

@implementation OONetworkBrowserViewManager

@synthesize updateNetwork, forceUpdateNetwork;

- (id)init {
    self = [super init];
    if (self) {
        // Create two arrays : The first is for the data source representation.
        // The second one contains temporary imported images  for thread safeness.
        networks = [[NSMutableArray alloc] init];
        importedNetworks = [[NSMutableArray alloc] init];
        self.updateNetwork = YES;
        //[importedNetworks addObject:[[OONetworkAddButton alloc] init]];
    }
    return self;
}

- (void)defineStyle {
    // Allow reordering, animations and set the dragging destination delegate.
    [networkBrowser setAllowsReordering:YES];
    //[networkBrowser setAnimates:YES];
    [networkBrowser setAllowsDroppingOnItems:YES];
    [networkBrowser setDraggingDestinationDelegate:self];
    [networkBrowser registerForDraggedTypes:[NSArray arrayWithObjects: NSPasteboardTypeString, NSFilenamesPboardType, nil]];
	
	// customize the appearance
    [networkBrowser setAllowsMultipleSelection:NO];
	[networkBrowser setCellsStyleMask:IKCellsStyleTitled | IKCellsStyleOutlined];
	
	// background layer
	//OONetworkBrowserBackgroundLayer *backgroundLayer = [[OONetworkBrowserBackgroundLayer alloc] init];
	//[networkBrowser setBackgroundLayer:backgroundLayer];
	//backgroundLayer.owner = networkBrowser;
	
	//-- change default font 
	// create a centered paragraph style
	NSMutableParagraphStyle *paraphStyle = [[NSMutableParagraphStyle alloc] init];
	[paraphStyle setLineBreakMode:NSLineBreakByTruncatingTail];
	[paraphStyle setAlignment:NSCenterTextAlignment];
	
	NSMutableDictionary *attributes = [[NSMutableDictionary alloc] init];	
	[attributes setObject:[NSFont systemFontOfSize:9] forKey:NSFontAttributeName]; 
	[attributes setObject:paraphStyle forKey:NSParagraphStyleAttributeName];	
	[attributes setObject:[NSColor blackColor] forKey:NSForegroundColorAttributeName];
	[networkBrowser setValue:attributes forKey:IKImageBrowserCellsTitleAttributesKey];
	
	attributes = [[NSMutableDictionary alloc] init];	
	[attributes setObject:[NSFont boldSystemFontOfSize:9] forKey:NSFontAttributeName]; 
	[attributes setObject:paraphStyle forKey:NSParagraphStyleAttributeName];	
	[attributes setObject:[NSColor whiteColor] forKey:NSForegroundColorAttributeName];
	
	//[networkBrowser setValue:attributes forKey:IKImageBrowserCellsHighlightedTitleAttributesKey];
	
	//change intercell spacing
	[networkBrowser setIntercellSpacing:NSMakeSize(20, 20)];
	
	//change selection color
	[networkBrowser setValue:[NSColor colorWithCalibratedRed:1 green:0 blue:0.5 alpha:1.0] forKey:IKImageBrowserSelectionColorKey];
	
	//set initial zoom value
	[networkBrowser setZoomValue:0.3];
    
}

- (void)updateDatasource
{
    NSLog(@"Update data source");
    // Update the datasource, add recently imported items.
    [networks addObjectsFromArray:importedNetworks];
	
	// Empty the temporary array.
    NSMutableArray* usersToAdd = [[NSMutableArray alloc] init];
    for (int i = 0; i < [importedNetworks count]; i++) {
        NSArray* members = ((OONetworkModel*)[importedNetworks objectAtIndex:i]).members;
        [usersToAdd addObjectsFromArray:members];
    }
    [userBrowserViewManager addUsersWithIds:usersToAdd];
    [importedNetworks removeAllObjects];
    [self updateUsers];
    // Reload the image browser, which triggers setNeedsDisplay.
    [networkBrowser reloadData];
}

- (void)updateUsers {
    for (int i = 0; i < [networks count]; i++) {
        [self updateModel:(OONetworkModel*)[networks objectAtIndex:i] InfoWithId:((OONetworkModel*)[networks objectAtIndex:i]).uid];
        // if selected update filtered users...
    }
    
}

#pragma mark -
#pragma mark import images from file system

- (BOOL)containsUserId:(NSString*)arg1
{
    for (OONetworkModel* str in importedNetworks) {
        if ([str.uid isEqualToString:arg1])
            return YES;
    }
    for (OONetworkModel* str in networks) {
        if ([str.uid isEqualToString:arg1])
            return YES;
    }
    return NO; 
}

- (OONetworkModel*)updateModel:(OONetworkModel*)arg1 InfoWithId:(NSString*)arg2 {
    arg1.name = [[OOPhone getInstance] getNetworkNameWithId:arg2];
    arg1.image = [NSImage imageNamed:NSImageNameNetwork];
    arg1.uid = arg2;
    arg1.members = [[OOPhone getInstance] getNetworkUsersWithNetworkId:arg2];
    arg1.mountPoint = [[OOPhone getInstance] getNetworkMountPointWithId:arg2];
    return arg1;
}

- (OONetworkModel*)getModelInfoWithId:(NSString*)arg1 {
    OONetworkModel* p = [[OONetworkModel alloc] init];
    [self updateModel:p InfoWithId:arg1];
    return p;
}

- (void)addNetworkWithId:(NSString*)arg1
{   
	BOOL addObject = NO;
    
	if (![self containsUserId:arg1]) {
		addObject = YES;
	}
	
	if (addObject) {
		// Add a path to the temporary images array.
        OONetworkModel* p = [self getModelInfoWithId:arg1];
		[importedNetworks addObject:p];
	}
}

- (void)forceUpdateNetworks {
    NSLog(@"Force update network");
    self.forceUpdateNetwork = YES;
}

- (void)updateNetworksLoop
{
    int updateNetworkFrequency = 10;
    static int updateNetworksLoop = 10;
    while (self.updateNetwork) {
        if (self.forceUpdateNetwork) {
            updateNetworksLoop += updateNetworkFrequency;
            self.forceUpdateNetwork = NO;
        }
        if (updateNetworksLoop >= updateNetworkFrequency) {
            NSLog(@"Update network");
            [self updateNetworks];
            updateNetworksLoop = 0;
        } else {
            updateNetworksLoop++;
            sleep(1);
        }
    }
}

- (void)perfomeUpdateNetworks:(NSArray*)arg1{
    NSInteger i, n;
    n = [arg1 count];
    for (i = 0; i < n; i++)
    {
        NSString* networkId = [arg1 objectAtIndex:i];
        [self addNetworkWithId:networkId];
    }
    
    // Update the data source in the main thread.
    [self performSelectorOnMainThread:@selector(updateDatasource) withObject:nil waitUntilDone:YES];
}

- (void)updateNetworks
{
    [[OOPhone getInstance] getUserNetworksAndPerformSelector:@selector(perfomeUpdateNetworks:) forObject:self];
}


#pragma mark -
#pragma mark IKImageBrowserDataSource

- (NSUInteger)numberOfItemsInImageBrowser:(IKImageBrowserView*)view
{
	// The item count to display is the datadsource item count.
    return [networks count];
}

- (id)imageBrowser:(IKImageBrowserView *) view itemAtIndex:(NSUInteger) index
{
    return [networks objectAtIndex:index];
}

- (void)imageBrowser:(IKImageBrowserView*)view removeItemsAtIndexes: (NSIndexSet*)indexes
{
	[networks removeObjectsAtIndexes:indexes];
}

- (BOOL)imageBrowser:(IKImageBrowserView*)view moveItemsAtIndexes: (NSIndexSet*)indexes toIndex:(unsigned int)destinationIndex
{
	NSInteger		index;
	NSMutableArray*	temporaryArray;
    
	temporaryArray = [[NSMutableArray alloc] init];
    
	// First remove items from the data source and keep them in a temporary array.
	for (index = [indexes lastIndex]; index != NSNotFound; index = [indexes indexLessThanIndex:index])
	{
		if (index < destinationIndex)
            destinationIndex --;
        
		id obj = [networks objectAtIndex:index];
		[temporaryArray addObject:obj];
		[networks removeObjectAtIndex:index];
	}
    
	// Then insert the removed items at the appropriate location.
	NSInteger n = [temporaryArray count];
	for (index = 0; index < n; index++)
	{
		[networks insertObject:[temporaryArray objectAtIndex:index] atIndex:destinationIndex];
	}
    
	return YES;
}

- (NSUInteger) imageBrowser:(IKImageBrowserView *) aBrowser writeItemsAtIndexes:(NSIndexSet *) itemIndexes toPasteboard:(NSPasteboard *)pasteboard {
    return 0;
}

#pragma mark -
#pragma mark drag n drop 

// -------------------------------------------------------------------------
//	draggingEntered:sender
// ------------------------------------------------------------------------- 
- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender {
    return NSDragOperationNone;
}
 
// -------------------------------------------------------------------------
//	draggingUpdated:sender
// ------------------------------------------------------------------------- 
- (NSDragOperation)draggingUpdated:(id <NSDraggingInfo>)sender {
    NSPasteboard *pboard;
    id dsource;
    
    dsource = [sender draggingSource];
    pboard = [sender draggingPasteboard];
    
    if (networkBrowser.dropOperation != IKImageBrowserDropOn)
        return NSDragOperationNone;
    else if (dsource == userBrowser) {
        NSUInteger networkIndex;
        networkIndex = networkBrowser.indexAtLocationOfDroppedItem;
        OONetworkModel* networkModel = [[networkBrowser cellForItemAtIndex:networkIndex] representedItem];
        NSIndexSet* indexSet = [userBrowser selectionIndexes];
        if ([indexSet count] > 0){
            OOUserModel* userModel = [[userBrowser cellForItemAtIndex:[indexSet firstIndex]] representedItem];
            if (![networkModel.members containsObject:userModel.uid]) {
                return NSDragOperationCopy;
            }
        }
        return NSDragOperationNone;
    }
    else if ([[pboard types] containsObject:NSURLPboardType])
        return NSDragOperationCopy;
    else
        return NSDragOperationNone;
}

// -------------------------------------------------------------------------
//	performDragOperation:sender
// ------------------------------------------------------------------------- 
- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender {
    NSPasteboard *pboard;
    id dsource;
    dsource = [sender draggingSource];
    pboard = [sender draggingPasteboard];
    
    NSUInteger networkIndex;
    networkIndex = networkBrowser.indexAtLocationOfDroppedItem;
    OONetworkModel* networkModel = [[networkBrowser cellForItemAtIndex:networkIndex] representedItem];
    
    if (dsource == userBrowser) {
        NSIndexSet* indexSet = [userBrowser selectionIndexes];
        if ([indexSet count] > 0){
            OOUserModel* userModel = [[userBrowser cellForItemAtIndex:[indexSet firstIndex]] representedItem];
            [[OOPhone getInstance] addUser:userModel.uid toNetwork:networkModel.uid];
        }
    } else if ([[pboard types] containsObject:NSURLPboardType]) {
        NSArray *sourcePaths;
        NSString *source;
        sourcePaths = [pboard propertyListForType: NSFilenamesPboardType];
        source = [[sourcePaths objectAtIndex: 0] stringByDeletingLastPathComponent];
        NSMutableArray *files = [NSMutableArray arrayWithCapacity: 1];
        for(int i = 0; i < [sourcePaths count]; i++) {
            [files addObject: [[sourcePaths objectAtIndex: i] lastPathComponent]];
        }
        [[NSWorkspace sharedWorkspace] performFileOperation:NSWorkspaceCopyOperation source:source destination:[networkModel.mountPoint path] files:files tag:nil];
        
    }
    
    return YES;
}

- (void)copyFiles {
    
}

// -------------------------------------------------------------------------
//	IKImageBrowserDelegate Protocol
// -------------------------------------------------------------------------
- (void) imageBrowserSelectionDidChange:(IKImageBrowserView *) aBrowser {
    NSIndexSet* indexSet = [aBrowser selectionIndexes];
    if ([indexSet count] > 0){
        OONetworkModel* networkModel = [[networkBrowser cellForItemAtIndex:[indexSet firstIndex]] representedItem];
        [userBrowserViewManager setFilteredUsers:networkModel.members];
    } else {
        [userBrowserViewManager setFilteredUsers:nil];
    }
}

- (void) imageBrowser:(IKImageBrowserView *) aBrowser cellWasDoubleClickedAtIndex:(NSUInteger) index {
    NSIndexSet* indexSet = [aBrowser selectionIndexes];
    if ([indexSet count] > 0){
        OONetworkModel* networkModel = [[networkBrowser cellForItemAtIndex:[indexSet firstIndex]] representedItem];
        [[NSWorkspace sharedWorkspace] openFile:[networkModel.mountPoint path]];
    }
}

@end
