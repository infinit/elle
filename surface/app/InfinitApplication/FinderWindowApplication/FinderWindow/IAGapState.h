//
//  IAGapState.h
//  FinderWindowApplication
//
//  Created by infinit on 10/29/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "gap.h"


#define IA_GAP_EVENT_LOGIN_OPERATION                    @"IA_GAP_EVENT_LOGIN_OPERATION"
#define IA_GAP_EVENT_USER_STATUS_NOTIFICATION           @"IA_GAP_EVENT_USER_STATUS_NOTIFICATION"
#define IA_GAP_EVENT_TRANSACTION_NOTIFICATION           @"IA_GAP_EVENT_TRANSACTION_NOTIFICATION"
#define IA_GAP_EVENT_TRANSACTION_STATUS_NOTIFICATION    @"IA_GAP_EVENT_TRANSACTION_STATUS_NOTIFICATION"

@interface IAUser : NSObject

+ (IAUser*)userWithId:(NSString*)user_id;

@property (retain)              NSString*       user_id;
@property (nonatomic, readonly) gap_UserStatus  status;
@property (nonatomic, readonly) NSString*       fullname;
@property (nonatomic, readonly) NSString*       email;

@end

@interface IATransaction : NSObject

@property (retain)              NSString*               transaction_id;
@property                       BOOL                    is_new;

@property (nonatomic, readonly) NSUInteger              files_count;
@property (nonatomic, readonly) NSUInteger              total_size;
@property (nonatomic, readonly) BOOL                    is_directory;

@property (nonatomic, readonly) NSString*               first_filename;
@property (nonatomic, readonly) NSString*               network_id;
@property (nonatomic, readonly) NSString*               sender_id;
@property (nonatomic, readonly) NSString*               sender_fullname;
@property (nonatomic, readonly) NSString*               recipient_fullname;

@property (nonatomic, readonly) gap_TransactionStatus   status;

@end


/**
 * @brief An operation return value
 */
@interface IAGapOperationResult : NSObject

@property (retain) NSString* data;
@property (nonatomic, readonly) gap_Status status;
@property (nonatomic, readonly) BOOL success;
@property (nonatomic, readonly) BOOL error;

@end

@interface IAGapState : NSOperationQueue

/**
 * @brief Logged in status.
 */

@property BOOL logged_in;


@property (readonly, nonatomic) NSString* token;
@property (readonly, nonatomic) NSString* self_id;

/**
 * @brief Returns a new IAGapState instance with a valid token.
 */
+ (IAGapState*) instanceWithToken:(NSString*)token;


/**
 * @brief Connect to the notification server. 
 */
- (void) startPolling;

- (BOOL) launchWatchdog;

/**
 * When IA_GAP_SINGLETON is defined, the class is usable as a singleton
 * and provide login/register features.
 */

#ifdef IA_GAP_SINGLETON

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



/**
 * @brief Register to meta.
 */
- (void)                register_:(NSString*)login
                     withFullname:(NSString*)fullname
                      andPassword:(NSString*)password
                    andDeviceName:(NSString*)device_name
                andActivationCode:(NSString*)activation_code
                  performSelector:(SEL)selector
                         onObject:(id)object;

- (void)               sendFiles:(NSArray*)files
                          toUser:(NSString*)user
                 performSelector:(SEL)selector
                        onObject:(id)object;

#endif


/**
 * @brief Search users that match a string.
 */
- (NSOperation*)searchUsers:(NSString*)str
            performSelector:(SEL)selector
                   onObject:(id)object;

/**
 * @brief Accept transaction.
 */
- (void)       acceptTransaction:(IATransaction*)transaction
                 performSelector:(SEL)selector
                        onObject:(id)object;


/**
 * @brief Reject transaction (refuse to download a file).
 */
- (void)       rejectTransaction:(IATransaction*)transaction
                 performSelector:(SEL)selector
                        onObject:(id)object;


/**
 * @brief Cancel transaction (stop or cancel a transaction from self).
 */
- (void)       cancelTransaction:(IATransaction*)transaction
                 performSelector:(SEL)selector
                        onObject:(id)object;

@end
