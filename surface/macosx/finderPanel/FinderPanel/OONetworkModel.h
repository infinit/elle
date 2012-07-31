//
//  OONetworkModel.h
//  FinderPanel
//
//  Created by Charles Guillot on 05/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Quartz/Quartz.h>

@interface OONetworkModel : NSObject <NSCoding, NSPasteboardWriting, NSPasteboardReading> 
{
    NSString*   uid;
    NSImage*    image; 
    NSString*   name;
    NSArray*    members;
}
@property NSString* uid;
@property NSImage*  image;
@property NSString* name;
@property NSArray*  members;
@end
