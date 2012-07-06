//
//  OOUserObject.h
//  FinderPanel
//
//  Created by Charles Guillot on 04/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Quartz/Quartz.h>

@interface OOUserModel : NSObject
{
    NSString* uid;
    NSImage* image; 
    NSString* fullName;
}
@property NSString* uid;
@property NSImage* image;
@property NSString* fullName;
@end