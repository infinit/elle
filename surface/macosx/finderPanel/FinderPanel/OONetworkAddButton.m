//
//  OONetworkAddButton.m
//  FinderPanel
//
//  Created by Charles Guillot on 18/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OONetworkAddButton.h"

@implementation OONetworkAddButton

@synthesize uid;
@synthesize image;
@synthesize name;

- (id)init {
    
    self = [super init];
    if (self) {
        self.image = [NSImage imageNamed:NSImageNameDotMac];
    }
    
    return self;
}

- (NSString*)imageRepresentationType {
	return IKImageBrowserNSImageRepresentationType;
}
- (id)imageRepresentation {
	return self.image;
}
- (NSString*)imageUID {
    return self.uid;
}
- (NSString*)imageTitle {
    return nil;
}
- (NSString*)imageSubtitle {
    return nil;
}

@end