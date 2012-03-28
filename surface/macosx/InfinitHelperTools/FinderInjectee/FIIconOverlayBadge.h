//
//  IHFDrawIconWithFrame.h
//  InfinitHelperTools
//
//  Created by Charles Guillot - Infinit on 3/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "FISwizzle.h"
#import "CDStructures.h"

@interface FIIconOverlayBadge : NSObject {
    NSMutableDictionary *nodeStatus; // fake ivar
    struct OpaqueIconRef *watingStatusImgRef;
    struct OpaqueIconRef *satusCheckedImgRef;
}

@property(readonly) struct OpaqueIconRef *satusCheckedImgRef;

+ (FIIconOverlayBadge*) instance;
- (IconRef)iconRefForURL:(NSURL *)aURL;
- (id) NodeObserver:(id)tIconAndTextCell;
- (void) removeNodeObserver:(id)tIconAndTextCell;
- (void) addNodeObserver:(id)tIconAndTextCell;
- (void) UpdateViewLayout;
@end
