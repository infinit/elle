//
//  AddInfinitToFavorite.m
//  HelperTools
//
//  Created by Charles Guillot - Infinit on 3/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FIAddInfinitToFavorite.h"

@implementation FIAddInfinitToFavorite

+(void)initialize
{
	[FIAddInfinitToFavorite addToFavorite];
}

- (id) init
{
    self = [super init];

    if(self) {
		// Init IVars
	}
    return self;
}


+ (FIAddInfinitToFavorite*) instance
{
    static FIAddInfinitToFavorite *item = nil;
	
	if(item == nil)
		item = [[FIAddInfinitToFavorite alloc] init];
	
	return item;
}

+ (void) addToFavorite
{
    //id MyVirtualNode = [NSClassFromString(@"NSNavVirtualNode") containerNode];
    //[MyVirtualNode setPath:@"/InfinitNode"];
    
    TFENode* node = [FIAddInfinitToFavorite feNodeWithPath:@"/opt"];
    [[FIAddInfinitToFavorite nodeRefWithFENode:node] addToFavoritesAtIndex:1];
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
