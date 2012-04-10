//
//  AddInfinitToFavorite.m
//  HelperTools
//
//  Created by Charles Guillot - Infinit on 3/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOFavorites.h"

@implementation OOFavorites

@synthesize sideBarImage;

+(void)initialize
{
	[OOFavorites addToFavorite];
    
    [NSClassFromString(@"TSidebarItemCell") 
     fi_swizzleMethod:@selector(drawWithFrame:inView:) 
     withMethod:@selector(drawWithFrame2:inView:) 
     error:NULL];
}

- (id) init
{
    self = [super init];

    if(self) {
		NSString* imageName;
        
        imageName = [[NSBundle bundleWithIdentifier:@"io.infinit.FinderDopant"] pathForResource:@"ok" ofType:@"icns"];
        NSImage *image = [[NSImage alloc] initWithContentsOfFile:imageName];
        sideBarImage = [[NSClassFromString(@"NSSidebarImage") alloc] initWithSourceImage:image];
        [image release];
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

+ (void) addToFavorite
{
    //id MyVirtualNode = [NSClassFromString(@"NSNavVirtualNode") containerNode];
    //[MyVirtualNode setPath:@"/InfinitNode"];
    
    TFENode* node = [OOFavorites feNodeWithPath:@"/opt"];
    [[OOFavorites nodeRefWithFENode:node] addToFavoritesAtIndex:1];
}

+(id)nodeRefWithFENode:(TFENode*)fn
{
	if(fn == nil)
		return nil;
	
	return [NSClassFromString(@"NSNavFBENode") _nodeWithFBENode:fn->fNodeRef];
}

+(TFENode*)feNodeWithNodeRef:(void*)fn
{
	TFENode *nouveauNode = (TFENode*)malloc(sizeof(TFENode));
	nouveauNode->fNodeRef = fn;
	return nouveauNode;
}

+(TFENode*)feNodeWithPath:(NSString*)path
{
	TFENode *nouveauNode = (TFENode*)malloc(sizeof(TFENode));
	nouveauNode->fNodeRef = [[NSClassFromString(@"NSNavFBENode") nodeWithPath:path] fbeNode];
	return nouveauNode;
}

+(TFENode*)feNodeWithFINode:(id)nd
{
	TFENode *nouveauNode = (TFENode*)malloc(sizeof(TFENode));
	nouveauNode->fNodeRef = [nd nodeRef];
	return nouveauNode;
}
@end

@implementation NSTextFieldCell (OOFavorites)

- (void)drawWithFrame2:(CGRect)arg1 inView:(id)arg2
{
    if ([[[self accessibilityAttributeValue:@"AXURL"] path] isEqualToString:@"/opt"])
    {
        [self setStringValue:@"infinit.io"];
        [self setImage:[OOFavorites instance].sideBarImage];
    }
    [self drawWithFrame2:arg1 inView:arg2];
}

@end
