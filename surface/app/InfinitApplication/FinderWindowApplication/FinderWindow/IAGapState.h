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

@interface IAUserStatusNotification : NSObject

@property (retain) NSString*    user_id;
@property NSInteger             status;

@end

@interface IATransactionNotification : NSObject

@property (retain) NSString*    first_filename;
@property NSUInteger            files_count;
@property NSUInteger            total_size;
@property BOOL                  is_directory;

@property (retain) NSString*    network_id;
@property (retain) NSString*    sender_id;
@property (retain) NSString*    sender_fullname;
@property (retain) NSString*    transaction_id;

@end

@interface IATransactionStatusNotification : NSObject

@property (retain) NSString*    transaction_id;
@property (retain) NSString*    network_id;
@property (retain) NSString*    sender_device_id;
@property (retain) NSString*    recipient_device_id;
@property (retain) NSString*    recipient_device_name;
@property gap_TransactionStatus status;

@end

/**
 * @brief An operation return value
 */
@interface IAGapOperationResult : NSObject

-(BOOL)success;
-(BOOL)error;

@end

@interface IAGapState : NSOperationQueue

/**
 * @brief Logged in status.
 */

@property BOOL logged_in;


@property (readonly, nonatomic) NSString* token;


/**
 * @brief Returns a new IAGapState instance with a valid token.
 */
+ (IAGapState*) instanceWithToken:(NSString*)token;


/**
 * @brief Connect to the notification server. 
 */
- (void)startPolling;

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
 * @brief Accept transaction.
 */
- (void)       acceptTransaction:(IATransactionNotification*)notif
                 performSelector:(SEL)selector
                        onObject:(id)object;


@end
