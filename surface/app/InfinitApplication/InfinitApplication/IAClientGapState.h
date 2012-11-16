//
//  IAClientGapState.h
//  InfinitApplication
//
//  Created by infinit on 11/15/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <FinderWindow/IAGapState.h>

@interface IAClientGapState : NSObject

/**
 * @brief Initialize a gap state with previously aquired token.
 *
 * @warning This function has to be called before everything else.
 */
+ (void) initGapInstanceWithToken:(NSString*)token;

/**
 * @brief Returns TRUE when the gap state was initialized and 
 *        successfully logged in.
 */
+ (BOOL) ready;

/**
 * @brief Retrieve the gap instance.
 *
 * @warning initGapInstance:withToken: should have been called.
 */
+ (IAGapState*) gap_instance;

@end
