//
//  OONodeHelper.m
//  medecine
//
//  Created by Charles Guillot on 11/04/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OONodeHelper.h"

@implementation OONodeHelper

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
