//
//  IAGapState.m
//  FinderWindowApplication
//
//  Created by infinit on 10/29/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAGapState.h"

#import "gap.h"

//- Timer operation -----------------------------------------------------------

@interface TimerOperation : NSOperation
{
@private
    NSTimer*    _timer;
    double      _interval;
    SEL         _selector;
    id          _object;
}

@property (nonatomic, readonly) BOOL isExecuting;
@property (nonatomic, readonly) BOOL isFinished;

@end

@implementation TimerOperation

@synthesize isExecuting = _executing;
@synthesize isFinished  = _finished;

- (id) initWithInterval:(double)seconds
        performSelector:(SEL)selector
               onObject:(id)object
{
    if ((self = [super init])) {
        _executing = NO;
        _finished  = NO;
        _timer = nil;
        _interval = seconds;
        _selector = selector;
        _object = object;
    }
    
    return self;
}

- (BOOL) isConcurrent {
    return YES;
}

- (void) finish {
    [self willChangeValueForKey:@"isFinished"];
    [self willChangeValueForKey:@"isExecuting"];
    _executing = NO;
    _finished = YES;
    [self didChangeValueForKey:@"isExecuting"];
    [self didChangeValueForKey:@"isFinished"];
}

- (void) start {
    if ([self isCancelled]) {
        [self willChangeValueForKey:@"isFinished"];
        _finished = YES;
        [self didChangeValueForKey:@"isFinished"];
    } else {
        [self willChangeValueForKey:@"isExecuting"];
        [self performSelectorOnMainThread:@selector(main)
                               withObject:nil
                            waitUntilDone:NO];
        _executing = YES;
        [self didChangeValueForKey:@"isExecuting"];
    }
}

- (void) timerFired:(NSTimer*)timer
{
    if (![self isCancelled])
    {
        [_object performSelector:_selector];
        [self finish];
    }
}

- (void) main {
    _timer = [NSTimer scheduledTimerWithTimeInterval:1.0
                                              target:self
                                            selector:@selector(timerFired:)
                                            userInfo:nil
                                             repeats:NO];
}

- (void) cancel {
    [_timer invalidate];
    [super cancel];
}

@end

//- operation function bloc -----------------------------------------------------

// Block type to queue gap operation
typedef int(^gap_operation_t)(NSOperation*);

//- Operation result ------------------------------------------------------------

@interface IAGapOperationResult ()

- (id)initWithStatusCode:(gap_Status)status_code;
- (id)initWithStatusCode:(gap_Status)status_code
             andData:(NSString*)object_id;
@end


@implementation IAGapOperationResult

@synthesize status = _status;
@synthesize data = _data;

- (id)initWithStatusCode:(gap_Status)status_code
{
    self = [super init];
    if (self)
    {
        _status = status_code;
        _data = nil;
    }
    return self;
}

- (id)initWithStatusCode:(gap_Status)status_code
             andData:(NSString*)data
{
    self = [super init];
    if (self)
    {
        _status = status_code;
        _data = data;
    }
    return self;
    
}

-(BOOL)success
{
    return self.status == gap_ok;
}

-(BOOL)error
{
    return !self.success;
}

@end



//- Callbacks for notifications -----------------------------------------------------



static void on_user_status(char const* user_id,
                           gap_UserStatus status);
static void on_transaction(char const* transaction_id,
                           int is_new);
static void on_transaction_status(char const* transaction);

//- Gap state additions -------------------------------------------------------------

@interface IAGapState ()
{
    gap_State* _state;
    BOOL _logged_in;
    BOOL _polling;
}


@property gap_State* state;

@end

//- Gap state implementation --------------------------------------------------------

@implementation IAGapState

@synthesize state = _state;
@synthesize logged_in = _logged_in;

+ (IAGapState*) instanceWithToken:(NSString*)token
{
    return [[IAGapState alloc] initWithToken:token];
}

+ (IAGapState*) instance
{
    static IAGapState* instance = NULL;

    if (instance == NULL)
        instance = [[IAGapState alloc] init];
    return instance;
}

+ (NSString*)_downloadDirectory
{
    return [NSHomeDirectory() stringByAppendingPathComponent:@"Downloads"];
}

- (id) initWithToken:(NSString*)token
{
    gap_State* state = gap_new_with_token([token UTF8String]);
    if (state == nil)
    {
        NSLog(@"ERROR: Cannot initialize gap with token");
        return nil;
    }
    
    gap_set_output_dir(state, [[IAGapState _downloadDirectory] UTF8String]);
    
    self = [super init];
    if (self == nil)
    {
        gap_free(state);
        return nil;
    }
    _state = state;
    _logged_in = TRUE;
    _polling = FALSE;
    return self;
}

- (NSString*) token
{
    if (!_state)
        return nil;
    return [[NSString alloc] initWithUTF8String:gap_user_token(_state)];
}

- (NSString*) self_id
{
    if (!_state)
        return nil;
    return [[NSString alloc] initWithUTF8String:gap_self_id(_state)];
}

-(id) init
{
    gap_State* state = gap_new();
    if (state == nil)
    {
        NSLog(@"ERROR: Cannot initialize gap");
        return nil;
    }

    self = [super init];
    
    if (!self)
    {
        gap_free(state);
        return nil;
    }
    
    gap_set_output_dir(state, [[IAGapState _downloadDirectory] UTF8String]);
    
    _state = state;
    _logged_in = FALSE;
    _polling = FALSE;
    return self;
}

// Wrapper that is only called from _startPolling or a timer
- (void)_poll
{
    if (!_polling || !_logged_in)
        return;
    gap_Status ret = gap_poll(_state);
    if (ret != gap_ok)
        NSLog(@"Warning: gap_poll failed");
    [self addOperation:[[TimerOperation alloc] initWithInterval:1
                                                performSelector:@selector(_poll)
                                                       onObject:self]];
}

// Start polling trophonius
- (void)startPolling
{
    @synchronized(self)
    {
        if (!_polling && _logged_in)
        {
            if ((gap_user_status_callback(self.state, &on_user_status) == gap_ok) &&
                (gap_transaction_callback(self.state, &on_transaction) == gap_ok) &&
                (gap_transaction_status_callback(self.state, &on_transaction_status) == gap_ok))
            {
                if (gap_pull_notifications(self.state, 10, 0) != gap_ok)
                {
                    NSLog(@"WARNING: Couldn't fetch notifications from meta");
                }
                if (gap_trophonius_connect(self.state) != gap_ok)
                {
                    NSLog(@"WARNING: Cannot connect to tropho !");
                    return;
                }
                _polling = TRUE;
                [self _poll];
            }
            else
                NSLog(@"WARNING: Cannot start polling");
        }
    }
}

- (BOOL) launchWatchdog
{
    if (gap_launch_watchdog(_state) != gap_ok)
    {
        NSLog(@"Couldn't launch the watchdog");
        return FALSE;
    }
    return TRUE;
}

//- Files ------------------------------------------------------------------------------------

- (void)               sendFiles:(NSArray*)files
                          toUser:(NSString*)user
                 performSelector:(SEL)selector
                        onObject:(id)object
{
    [self _addOperation:^gap_Status(NSOperation* op) {
        if (![files count])
        {
            return gap_error;
        }
        char const** cfiles = calloc([files count] + 1, sizeof(char*));
        if (cfiles == NULL)
            return gap_error; //XXX specialized error
        int i = 0;
        for (id file in files)
        {
            cfiles[i++] = [file UTF8String];
        }
        gap_Status res = gap_send_files(self.state, [user UTF8String], cfiles);
        free(cfiles);
        return res;
    } performSelector:selector onObject:object];
}

//- Transaction-------------------------------------------------------------------------------

- (void)       acceptTransaction:(IATransaction*)transaction
                 performSelector:(SEL)selector
                        onObject:(id)object
{
    [self _addOperation:^(NSOperation* op) {
        gap_Status res;
        res = gap_update_transaction(self.state,
                                     [transaction.transaction_id UTF8String],
                                     gap_transaction_status_accepted);
        return res;
    } performSelector:selector onObject:object withData:transaction];
}


- (void)       rejectTransaction:(IATransaction*)transaction
                 performSelector:(SEL)selector
                        onObject:(id)object
{
    [self _addOperation:^(NSOperation* op) {
        gap_Status res;
        res = gap_update_transaction(self.state,
                                     [transaction.transaction_id UTF8String],
                                     gap_transaction_status_canceled);
        return res;
    } performSelector:selector onObject:object withData:transaction];
}

- (void)       cancelTransaction:(IATransaction*)transaction
                 performSelector:(SEL)selector
                        onObject:(id)object
{
    [self _addOperation:^(NSOperation* op) {
        gap_Status res;
        res = gap_update_transaction(self.state,
                                     [transaction.transaction_id UTF8String],
                                     gap_transaction_status_canceled);
        return res;
    } performSelector:selector onObject:object withData:transaction];
}

//- User -------------------------------------------------------------------------------------



- (void)                login:(NSString*)login
                 withPassword:(NSString*)password
                andDeviceName:(NSString*)device_name
              performSelector:(SEL)selector
                     onObject:(id)object
{
    NSLog(@"Calling login method");
//    __weak id this = self;
    [self _addOperation:^(NSOperation* op) {
        NSLog(@"Starting LOGIN");
        char* hash_password = gap_hash_password(self.state,
                                                [login UTF8String],
                                                [password UTF8String]);
        int res = gap_login(self.state,
                            [login UTF8String],
                            hash_password);
        gap_hash_free(hash_password);
        if (res == gap_ok)
            res = gap_set_device_name(self.state, [device_name UTF8String]);
        
        if (res == gap_ok)
        {}
        else
            NSLog(@"Cannot login !");
        
        if (res == gap_ok)
        {
        }
        else
            NSLog(@"Cannot connect to tropho");
        
        if (res == gap_ok)
        {
            self.logged_in = TRUE;
        }
        else
            NSLog(@"Cannot register callbacks");
        NSLog(@"Login process result: %d", res);
        return res;
    } performSelector:selector onObject:object];
}

- (void)                register_:(NSString*)login
                     withFullname:(NSString*)fullname
                      andPassword:(NSString*)password
                    andDeviceName:(NSString*)device_name
                andActivationCode:(NSString*)activation_code
                  performSelector:(SEL)selector
                         onObject:(id)object
{
//    __weak id this = self;
    [self _addOperation:^(NSOperation* op) {
        gap_Status res;
        char* hash_password = gap_hash_password(self.state,
                                                [login UTF8String],
                                                [password UTF8String]);
        res = gap_register(self.state,
                           [fullname UTF8String],
                           [login UTF8String],
                           hash_password,
                           [device_name UTF8String],
                           [activation_code UTF8String]);
        
        if (res == gap_ok)
            res = gap_set_device_name(self.state,
                                      [device_name UTF8String]);
        if (res == gap_ok)
        {
            self.logged_in = true;
        }
        return res;
    } performSelector:selector onObject:object];
}


- (NSOperation*)searchUsers:(NSString*)str
            performSelector:(SEL)selector
                   onObject:(id)object
{
    NSMutableArray* user_array = [NSMutableArray array];
    return [self _addOperation:^(NSOperation* op) {
        char** users = gap_search_users(self.state, [str UTF8String]);
        if (users == NULL)
            return gap_error;
        if (user_array == nil)
            return gap_error;
        for (char** user = users; *user != NULL; ++user)
        {
            IAUser* full_user = [IAUser userWithId:[NSString stringWithUTF8String:*user]];
            [user_array addObject:full_user];
        }
        return gap_ok;
    }   performSelector:selector
               onObject:object
               withData:user_array];
}


// Wrap any operation in a block and execute it in the mail thread
-(NSOperation*) _addOperation:(gap_operation_t)operation
      performSelector:(SEL)selector
             onObject:(id)object
{
    return [self _addOperation:operation
               performSelector:selector
                      onObject:object
                      withData:nil];
}

-(NSOperation*) _addOperation:(gap_operation_t)operation
      performSelector:(SEL)selector
             onObject:(id)object
             withData:(id)data
{
    __block NSBlockOperation* block_operation =
    [NSBlockOperation blockOperationWithBlock:^(void) {
        int result = operation(block_operation);
        if ([block_operation isCancelled])
            return;
        id operation_result = [[IAGapOperationResult alloc] initWithStatusCode:result
                                                                       andData:data];
        if ([block_operation isCancelled])
            return;
        NSLog(@"Calling %@.%@", object, NSStringFromSelector(selector));
        [object performSelectorOnMainThread:selector
                                 withObject:operation_result
                              waitUntilDone:NO];
    }];
    
    [self addOperation:block_operation];
    return block_operation;
}


@end

//- User status notification implementation ---------------------------------

@implementation IAUser
{
@private
    gap_State* _state;
}

@synthesize user_id = _user_id;

- (gap_UserStatus)status
{
    return gap_user_status(_state, [_user_id UTF8String]);
}

- (NSString*)fullname
{
    return [NSString stringWithUTF8String:gap_user_fullname(_state, [_user_id UTF8String])];
}

- (NSString*)email
{
    return [NSString stringWithUTF8String:gap_user_email(_state, [_user_id UTF8String])];
}

- (id) init:(NSString*)user_id
{
    self = [super init];
    if (self)
    {
        NSLog(@"BUILD USER WITH %@", user_id);
        _user_id = user_id;
        _state = [IAGapState instance].state;
    }
    return self;
}

+ (IAUser*)userWithId:(NSString*)user_id
{
    return [[IAUser alloc] init:user_id];
}

@end

//- Transaction notification implementation -----------------------------------

@implementation IATransaction
{
@private
    gap_State* _state;
}


- (id) init:(NSString*)transaction_id
{
    self = [super init];
    if (self)
    {
        _transaction_id = transaction_id;
        _state = [IAGapState instance].state;
        _is_new = false;
    }
    return self;
}

+ (IATransaction*)transactionWithId:(NSString*)transaction_id
{
    return [[IATransaction alloc] init:transaction_id];
}

@synthesize transaction_id = _transaction_id;
@synthesize is_new = _is_new;

#define TR_GET(__attr)                                                  \
    gap_transaction_ ## __attr(_state, [_transaction_id UTF8String])    \
/**/

#define TR_WRAP(__ret, __attr)                                          \
- (__ret)__attr { return TR_GET(__attr); }                              \
/**/

#define TR_STR_WRAP(__attr)                                             \
- (NSString*)__attr {                                                   \
    if (TR_GET(__attr) != NULL)                                         \
        return [NSString stringWithUTF8String:TR_GET(__attr)];          \
    return [NSString stringWithFormat:@"invalid %s", #__attr];          \
}                                                                       \
/**/


TR_WRAP(NSUInteger, files_count)
TR_WRAP(NSUInteger, total_size)
TR_WRAP(BOOL, is_directory)
TR_WRAP(gap_TransactionStatus, status)

TR_STR_WRAP(first_filename)
TR_STR_WRAP(network_id)
TR_STR_WRAP(sender_fullname)
TR_STR_WRAP(sender_id)
TR_STR_WRAP(recipient_fullname);

@end

//- notif callback implementation -----------------------------------------------------------
static void on_user_status(char const* user_id,
                           gap_UserStatus status)
{
    assert(user_id != NULL);
    NSLog(@">>> on user status notif !");
    IAUser* user = [IAUser userWithId:[NSString stringWithUTF8String:user_id]];
    [[NSNotificationCenter defaultCenter] postNotificationName:IA_GAP_EVENT_USER_STATUS_NOTIFICATION
                                                        object:user];
}

static void on_transaction(char const* transaction_id,
                           int is_new)
{
    assert(transaction_id != NULL);
    NSLog(@">>> On transaction notif");
    IATransaction* transaction = [IATransaction transactionWithId:[NSString stringWithUTF8String:transaction_id]];
    transaction.is_new = is_new;
    [[NSNotificationCenter defaultCenter] postNotificationName:IA_GAP_EVENT_TRANSACTION_NOTIFICATION
                                                        object:transaction];
}

static void on_transaction_status(char const* transaction_id)
{
    assert(transaction_id != NULL);
    NSLog(@">>> On transaction status notif");
    IATransaction* transaction = [IATransaction transactionWithId:[NSString stringWithUTF8String:transaction_id]];
    [[NSNotificationCenter defaultCenter] postNotificationName:IA_GAP_EVENT_TRANSACTION_STATUS_NOTIFICATION
                                                        object:transaction];
}
