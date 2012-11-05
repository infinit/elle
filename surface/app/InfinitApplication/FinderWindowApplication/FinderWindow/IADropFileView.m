//
//  IADropFileView.m
//  FinderWindowApplication
//
//  Created by infinit on 11/5/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IADropFileView.h"

@implementation IADropFileView
{
    NSArray* _pending_files;
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        _pending_files = [[NSArray alloc] init];
        id dragged_types = [NSArray arrayWithObjects:NSFilenamesPboardType, nil];
        [self registerForDraggedTypes:dragged_types];
    }
    		
    return self;
}

//- (void)drawRect:(NSRect)dirtyRect
//{
//    // Drawing code here.
//}

/**
 * @brief Called when the mouse is over the zone and returns
 *        allowed operation.
 */
- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
    NSLog(@"draggingEntered");
    return [self _getDragOperation:sender];
}

// Returns allowed drag operation.
- (NSDragOperation)_getDragOperation:(id <NSDraggingInfo>)sender
{
    NSPasteboard *pboard  = [sender draggingPasteboard];
    NSDragOperation mask = [sender draggingSourceOperationMask];
    if ([[pboard types] containsObject:NSFilenamesPboardType] && (mask & NSDragOperationCopy))
    {
        return NSDragOperationCopy;
    }
    return NSDragOperationNone;
}


//
//- (void)pasteboard:(NSPasteboard*)sender
//              item:(NSPasteboardItem*)item
//provideDataForType:(NSString*)type
//{
//    /*------------------------------------------------------
//     method called by pasteboard to support promised
//     drag types.
//     --------------------------------------------------------*/
//    NSLog(@"pasteboard:pasteboard:provideDataForType:");
//    //sender has accepted the drag and now we need to send the data for the type we promised
//    if ( [type compare: NSPasteboardTypeTIFF] == NSOrderedSame ) {
//        
//        //set data for TIFF type on the pasteboard as requested
// //       [sender setData:[[self image] TIFFRepresentation] forType:NSPasteboardTypeTIFF];
//        
//    } else if ( [type compare: NSPasteboardTypePDF] == NSOrderedSame ) {
//        
//        //set data for PDF type on the pasteboard as requested
//        [sender setData:[self dataWithPDFInsideRect:[self bounds]] forType:NSPasteboardTypePDF];
//    }
//    
//}
//
//- (NSDragOperation)     draggingSession:(NSDraggingSession *)session
//  sourceOperationMaskForDraggingContext:(NSDraggingContext)context
//{
//    NSLog(@"draggingSession:sourceOperationMaskForDraggingContext:");
//    /*------------------------------------------------------
//     NSDraggingSource protocol method.  Returns the types of operations allowed in a certain context.
//     --------------------------------------------------------*/
//    switch (context) {
//        case NSDraggingContextOutsideApplication:
//            return NSDragOperationCopy;
//            
//            //by using this fall through pattern, we will remain compatible if the contexts get more precise in the future.
//        case NSDraggingContextWithinApplication:
//        default:
//            return NSDragOperationCopy;
//            break;
//    }
//}

/**
 * @brief Accept activation click as click in window, so source doesn't 
 *        have to be the active window
 */
- (BOOL)acceptsFirstMouse:(NSEvent*)event
{
    return YES;
}

//
//- (NSRect)windowWillUseStandardFrame:(NSWindow *)window defaultFrame:(NSRect)newFrame
//{
//    NSLog(@"(NSRect)windowWillUseStandardFrame:(NSWindow *)window defaultFrame:(NSRect)newFrame");
//    /*------------------------------------------------------
//     delegate operation to set the standard window frame
//     --------------------------------------------------------*/
//    //get window frame size
//    NSRect ContentRect=self.frame;
//    
//    //set it to the image frame size
////    ContentRect.size=[[self frame];
//    
//    return [NSWindow frameRectForContentRect:ContentRect styleMask: [window styleMask]];
//};

/**
 * @brief Determine if we can accept the drop.
 */
- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
{
    NSLog(@"prepareForDragOperation");
    
    [self setNeedsDisplay: YES];
    
    return TRUE;
}

/**
 * @brief Handle the drop operation.
 */
- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    if ([self _getDragOperation:sender] != NSDragOperationCopy)
    {
        NSLog(@"WARNING: The drag operation does not permit copying files ...");
        return FALSE;
    }
    NSPasteboard* pboard = [sender draggingPasteboard];
    NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];
    _pending_files = [[NSSet setWithArray:[_pending_files arrayByAddingObjectsFromArray:files]] allObjects];
    [self.label setStringValue:[NSString stringWithFormat:@"%ld files", [_pending_files count]]];
    [[self main_controller] refresh];
    return TRUE;
}

- (BOOL) hasPendingFiles
{
    return [_pending_files count] > 0;
}

@end
