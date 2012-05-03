//
//  OOFavorites.m
//  Dopant
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
		NSString* imagePath = [[NSBundle bundleWithIdentifier:@"io.infinit.FinderDopant"] pathForResource:@"sidebar-18" ofType:@"png"];
        self.sideBarImage = [[NSImage alloc] initWithContentsOfFile:imagePath];
        [self.sideBarImage setTemplate:YES];
        
        self.infinitMountPath = [[NSURL URLWithString:[NSHomeDirectory() stringByAppendingPathComponent:@"/.config/infinit/Infinit"]] absoluteString];

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
        [self setStringValue:@"Infinit"];
        if ([self respondsToSelector:@selector(image)])
        {
            id img = [self image];
            if ([img respondsToSelector:@selector(initWithSourceImage:)])
            {
                [img initWithSourceImage:[OOFavorites instance].sideBarImage];
                
            }
            else if ([self respondsToSelector:@selector(setImage:)])
            {
                [self setImage:[OOFavorites instance].sideBarImage];
            }
        }
    }
    [self drawWithFrame2:arg1 inView:arg2];
}

@end
