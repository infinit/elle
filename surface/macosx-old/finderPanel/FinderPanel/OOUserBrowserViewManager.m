#import "OOUserBrowserViewManager.h"
#import "OOUserBrowserBackgroundLayer.h"
#import "OOUserProfileImage.h"
#import "OOUserModel.h"
#import <Phone/OOPhone.h>

@implementation OOUserBrowserViewManager

- (id)init {
    self = [super init];
    if (self) {
        // Create two arrays : The first is for the data source representation.
        // The second one contains temporary imported images  for thread safeness.
        users = [[NSMutableArray alloc] init];
        importedUsers = [[NSMutableArray alloc] init];
        filteredUsers = [[NSMutableArray alloc] init];
        notFiltered = YES;
    }
    return self;
}

- (void)defineStyle {
    // Allow reordering, animations and set the dragging destination delegate.
    [userBrowser setAllowsReordering:YES];
    //[userBrowser setAnimates:YES];
    //[userBrowser setDraggingDestinationDelegate:self];
	
	// customize the appearance
	[userBrowser setCellsStyleMask:IKCellsStyleTitled];
    
	// background layer
	OOUserBrowserBackgroundLayer *backgroundLayer = [[OOUserBrowserBackgroundLayer alloc] init];
	[userBrowser setBackgroundLayer:backgroundLayer];
	backgroundLayer.owner = userBrowser;
	
	//-- change default font 
	// create a centered paragraph style
	NSMutableParagraphStyle *paraphStyle = [[NSMutableParagraphStyle alloc] init];
	[paraphStyle setLineBreakMode:NSLineBreakByTruncatingTail];
	[paraphStyle setAlignment:NSCenterTextAlignment];
	
	NSMutableDictionary *attributes = [[NSMutableDictionary alloc] init];	
	[attributes setObject:[NSFont systemFontOfSize:9] forKey:NSFontAttributeName]; 
	//[attributes setObject:paraphStyle forKey:NSParagraphStyleAttributeName];
	[attributes setObject:[NSColor blackColor] forKey:NSForegroundColorAttributeName];
	[userBrowser setValue:attributes forKey:IKImageBrowserCellsTitleAttributesKey];
	
	attributes = [[NSMutableDictionary alloc] init];	
	[attributes setObject:[NSFont boldSystemFontOfSize:9] forKey:NSFontAttributeName]; 
	[attributes setObject:paraphStyle forKey:NSParagraphStyleAttributeName];	
	[attributes setObject:[NSColor whiteColor] forKey:NSForegroundColorAttributeName];
	
	[userBrowser setValue:attributes forKey:IKImageBrowserCellsHighlightedTitleAttributesKey];
	
	//change intercell spacing
	[userBrowser setIntercellSpacing:NSMakeSize(20, 20)];
	
	//change selection color
	[userBrowser setValue:[NSColor colorWithCalibratedRed:1 green:0 blue:0.5 alpha:1.0] forKey:IKImageBrowserSelectionColorKey];
	
	//set initial zoom value
	[userBrowser setZoomValue:0.5];
    
}

- (void)updateDatasource
{
    // Update the datasource, add recently imported items.
    [users addObjectsFromArray:importedUsers];
	
	// Empty the temporary array.
    [importedUsers removeAllObjects];
    
    // Reload the image browser, which triggers setNeedsDisplay.
    [userBrowser reloadData];
}


#pragma mark -
#pragma mark import images from file system

- (BOOL)containsUserId:(NSString*)arg1
{
    for (OOUserModel* str in importedUsers) {
        if ([str.uid isEqualToString:arg1])
            return YES;
    }
    for (OOUserModel* str in users) {
        if ([str.uid isEqualToString:arg1])
            return YES;
    }
    return NO; 
}

- (OOUserModel*)getUserInfoWithId:(NSString*)arg1 {
    // Add a path to the temporary images array.
    OOUserModel* p = [[OOUserModel alloc] init];
    p.fullName =[[OOPhone getInstance] getUserFullNameById:arg1];
    p.image = [[OOUserProfileImage alloc] initWithGravatarEmail:[[OOPhone getInstance] getUserEMailById:arg1]];
    p.uid = arg1;
    return p;
}

- (void)addUserWithId:(NSString*)userId
{   
	BOOL addObject = NO;
    
	if (![self containsUserId:userId]) {
		addObject = YES;
	}
	
	if (addObject) {
        OOUserModel* p = [self getUserInfoWithId:userId];
		[importedUsers addObject:p];
	}
}

- (void)addUsersWithIds:(NSArray*)userIds
{   
    NSInteger i, n;
	n = [userIds count];
    for (i = 0; i < n; i++)
	{
        NSString* userId = [userIds objectAtIndex:i];
		[self addUserWithId:userId];
    }
    
	// Update the data source in the main thread.
    if (notFiltered) {
        [self updateDatasource];
        [self setFilteredUsers:nil];
        
        
    }
}


#pragma mark -
#pragma mark IKImageBrowserDataSource

- (NSUInteger)numberOfItemsInImageBrowser:(IKImageBrowserView*)view
{
	// The item count to display is the datadsource item count.
    return [filteredUsers count];
}

- (id)imageBrowser:(IKImageBrowserView *) view itemAtIndex:(NSUInteger) index
{
    return [filteredUsers objectAtIndex:index];
}

- (void)imageBrowser:(IKImageBrowserView*)view removeItemsAtIndexes: (NSIndexSet*)indexes
{
	[filteredUsers removeObjectsAtIndexes:indexes];
}

- (BOOL)imageBrowser:(IKImageBrowserView*)view moveItemsAtIndexes:(NSIndexSet*)indexes toIndex:(unsigned int)destinationIndex
{
	NSInteger		index;
	NSMutableArray*	temporaryArray;
    
	temporaryArray = [[NSMutableArray alloc] init];
    
	// First remove items from the data source and keep them in a temporary array.
	for (index = [indexes lastIndex]; index != NSNotFound; index = [indexes indexLessThanIndex:index])
	{
		if (index < destinationIndex)
            destinationIndex --;
        
		id obj = [filteredUsers objectAtIndex:index];
		[temporaryArray addObject:obj];
		[filteredUsers removeObjectAtIndex:index];
	}
    
	// Then insert the removed items at the appropriate location.
	NSInteger n = [temporaryArray count];
	for (index = 0; index < n; index++) {
		[filteredUsers insertObject:[temporaryArray objectAtIndex:index] atIndex:destinationIndex];
	}
    
	return YES;
}

- (NSUInteger) imageBrowser:(IKImageBrowserView *) aBrowser writeItemsAtIndexes:(NSIndexSet *) itemIndexes toPasteboard:(NSPasteboard *)pasteboard {
    NSMutableArray *draggedUsers = [[NSMutableArray alloc] init];
    [itemIndexes enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop)
     {
         OOUserModel *user = [userBrowser cellForItemAtIndex:idx].representedItem;
         [draggedUsers addObject:user.uid];
     }];
    if ([draggedUsers count] > 0) {
        [pasteboard clearContents];
        NSArray *array = [[NSArray alloc] initWithArray:draggedUsers];
        [pasteboard writeObjects:array];
    }
    return [draggedUsers count];
}

#pragma mark -
#pragma mark drag n drop 

// -------------------------------------------------------------------------
//	draggingEntered:sender
// ------------------------------------------------------------------------- 
- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
    return NSDragOperationCopy;
}

// -------------------------------------------------------------------------
//	draggingUpdated:sender
// ------------------------------------------------------------------------- 
- (NSDragOperation)draggingUpdated:(id <NSDraggingInfo>)sender
{
    return NSDragOperationCopy;
}

// -------------------------------------------------------------------------
//	performDragOperation:sender
// ------------------------------------------------------------------------- 
- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
   /* NSData*			data = nil;
    NSPasteboard*	pasteboard = [sender draggingPasteboard];

    // Look for paths on the pasteboard.
    if ([[pasteboard types] containsObject:NSFilenamesPboardType]) 
    data = [pasteboard dataForType:NSFilenamesPboardType];

    if (data)
    {
        NSString* errorDescription;

        // Retrieve  paths.
        NSArray* filenames = [NSPropertyListSerialization propertyListFromData:data 
        mutabilityOption:kCFPropertyListImmutable 
        format:nil 
        errorDescription:&errorDescription];

        // Add paths to the data source.
        //[self addImagesWithPaths:filenames];

        // Make the image browser reload the data source.
        [self updateDatasource];
    }*/

    // Accept the drag operation.
    return YES;
}

//
//
//

- (void)setFilteredUsers:(NSArray*)userIds {
    NSLog(@"Filter users");
    [filteredUsers removeAllObjects];
    if (userIds == nil) {
        notFiltered = YES;
        [filteredUsers addObjectsFromArray:users];
    } else {
        notFiltered = NO;
        for (int i = 0; i < [userIds count]; i ++) {
            [filteredUsers addObject:[self getUserInfoWithId:[userIds objectAtIndex:i]]];
        }
    }
    // Reload the image browser, which triggers setNeedsDisplay.
    [userBrowser reloadData];
}
@end
