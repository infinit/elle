//
//  AddInfinitToFavorite.m
//  HelperTools
//
//  Created by Charles Guillot - Infinit on 3/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOFavorites.h"
#import "OONodeHelper.h"

@implementation OOFavorites

@synthesize sideBarImage;
@synthesize infinitMountPath;

+(void)initialize
{
    [NSClassFromString(@"TSidebarItemCell") 
     fi_swizzleMethod:@selector(drawWithFrame:inView:) 
     withMethod:@selector(drawWithFrame2:inView:) 
     error:NULL];
}

- (id) init
{
    self = [super init];

    if(self) {
		NSString* imagePath = [[NSBundle bundleWithIdentifier:@"io.infinit.FinderDopant"] pathForResource:@"Infinit" ofType:@"icns"];
        NSImage *image = [[NSImage alloc] initWithContentsOfFile:imagePath];
        self.sideBarImage = [[NSClassFromString(@"NSSidebarImage") alloc] initWithSourceImage:image];
        
        self.infinitMountPath = [[NSURL URLWithString:[NSHomeDirectory() stringByAppendingPathComponent:@"/.config/infinit/mnt"]] absoluteString];

        // Add Favorite 
        // TODO: preference checked.
        [self addToFavorite];
    }
    return self;
}


+ (OOFavorites*) instance
{
    static OOFavorites *item = nil;
	
	if(item == nil)
		item = [[OOFavorites alloc] init];
	
	return item;
}

- (void) addToFavorite
{
    TFENode* node = [OONodeHelper feNodeWithPath:self.infinitMountPath];
    [[OONodeHelper nodeRefWithFENode:node] addToFavoritesAtIndex:1];
}


@end

@implementation NSTextFieldCell (OOFavorites)

- (void)drawWithFrame2:(CGRect)arg1 inView:(id)arg2
{
    if ([[[self accessibilityAttributeValue:@"AXURL"] path] isEqualToString:[OOFavorites instance].infinitMountPath])
    {
        [self setStringValue:@"infinit.io"];
        [self setImage:[OOFavorites instance].sideBarImage];
    }
    [self drawWithFrame2:arg1 inView:arg2];
}

@end
