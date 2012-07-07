//
//  OOUserObject.m
//  FinderPanel
//
//  Created by Charles Guillot on 04/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOUserModel.h"

@implementation OOUserModel

@synthesize uid;
@synthesize image;
@synthesize fullName;

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
    return self.fullName;
}
- (NSString*)imageSubtitle {
    return self.fullName;
}

@end