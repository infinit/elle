//
//  IAGapState.h
//  FinderWindowApplication
//
//  Created by infinit on 10/29/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Foundation/Foundation.h>

#define IA_GAP_EVENT_LOGGED_IN @"IA_GAP_EVENT_LOGGED_IN"

/**
 * @brief An operation return value
 */
@interface IAGapOperationResult : NSObject

-(BOOL)success;
-(BOOL)error;

@end

@interface IAGapState : NSOperationQueue

/** 
 * @brief Returns an IAGapState singleton.
 *
 * @note If you need multiple instance, you can create a new instance
 * of that class.
 */
+ (IAGapState*) instance;

/** 
 * @brief Login into meta.
 */
- (void)                login:(NSString*)login
                 withPassword:(NSString*)password
                andDeviceName:(NSString*)device_name
              performSelector:(SEL)selector
                     onObject:(id)object;


@end
