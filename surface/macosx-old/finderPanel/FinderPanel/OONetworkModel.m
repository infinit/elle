//
//  OONetworkModel.m
//  FinderPanel
//
//  Created by Charles Guillot on 05/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OONetworkModel.h"

@implementation OONetworkModel

@synthesize uid;
@synthesize image;
@synthesize name;
@synthesize members;
@synthesize mountPoint;

///
/// Unused methods
///

- (id)initWithCoder:(NSCoder *)aDecoder {
    return nil;
}

- (void)encodeWithCoder:(NSCoder *)aCoder {
}

+ (NSArray *)readableTypesForPasteboard:(NSPasteboard *)pasteboard {
    return nil;
}

///
///
///

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
    return self.name;
}

- (NSString*)imageSubtitle {
    return self.name;
}

@end
