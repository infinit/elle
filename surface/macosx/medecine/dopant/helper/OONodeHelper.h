//
//  OONodeHelper.h
//  medecine
//
//  Created by Charles Guillot on 11/04/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CDStructures.h"

@interface OONodeHelper : NSObject

+(id)nodeRefWithFENode:(TFENode*)fn;
+(TFENode*)feNodeWithNodeRef:(void*)fn;
+(TFENode*)feNodeWithPath:(NSString*)path;
+(TFENode*)feNodeWithFINode:(id)nd;

@end
