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
typedef int(^gap_operation_t)(void);

//- Operation result ------------------------------------------------------------

@interface IAGapOperationResult ()
{
    gap_Status status;
}

@property gap_Status status;

- (id)initWithStatusCode:(gap_Status)status_code;
- (id)initWithStatusCode:(gap_Status)status_code
             andObjectId:(NSString*)object_id;
@end


@implementation IAGapOperationResult

@synthesize status;
@synthesize data = _data;

- (id)initWithStatusCode:(gap_Status)status_code
{
    self = [super init];
    if (self)
    {
        self.status = status_code;
    }
    return self;
}

- (id)initWithStatusCode:(gap_Status)status_code
             andData:(NSString*)data
{
    self = [super init];
    if (self)
    {
        self.status = status_code;
        self.data = data;
    }
    return self;
    
}

-(BOOL)success
{
    return self.status == gap_ok;
}

-(BOOL)error
{
    return self.status != gap_ok;
}

@end



//- Callbacks for notifications -----------------------------------------------------



static void on_user_status(gap_UserStatusNotification const* n);
static void on_transaction(gap_TransactionNotification const* n);
static void on_transaction_status(gap_TransactionStatusNotification const* n);

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

- (id) initWithToken:(NSString*)token
{
    gap_State* state = gap_new_with_token([token UTF8String]);
    if (state == nil)
    {
        NSLog(@"ERROR: Cannot initialize gap with token");
        return nil;
    }
    
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
    [self _addOperation:^gap_Status(void) {
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

- (void)       acceptTransaction:(IATransactionNotification*)notif
                 performSelector:(SEL)selector
                        onObject:(id)object
{
    [self _addOperation:^(void) {
        gap_Status res;
        res = gap_update_transaction(self.state,
                                     [notif.transaction_id UTF8String],
                                     gap_transaction_status_accepted);
        return res;
    } performSelector:selector onObject:object withData:notif];
}


- (void)       rejectTransaction:(IATransactionNotification*)notif
                 performSelector:(SEL)selector
                        onObject:(id)object
{
    [self _addOperation:^(void) {
        gap_Status res;
        res = gap_update_transaction(self.state,
                                     [notif.transaction_id UTF8String],
                                     gap_transaction_status_rejected);
        return res;
    } performSelector:selector onObject:object withData:notif];
}

- (void)       cancelTransaction:(IATransactionNotification*)notif
                 performSelector:(SEL)selector
                        onObject:(id)object
{
    [self _addOperation:^(void) {
        gap_Status res;
        res = gap_update_transaction(self.state,
                                     [notif.transaction_id UTF8String],
                                     gap_transaction_status_canceled);
        return res;
    } performSelector:selector onObject:object withData:notif];
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
    [self _addOperation:^(void) {
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
    [self _addOperation:^(void) {
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




// Wrap any operation in a block and execute it in the mail thread
-(void) _addOperation:(gap_operation_t)operation
      performSelector:(SEL)selector
             onObject:(id)object
{
    [self addOperationWithBlock:^(void) {
        int result = operation();
        [object performSelectorOnMainThread:selector
                                 withObject:[[IAGapOperationResult alloc] initWithStatusCode:result]
                              waitUntilDone:NO];
    }];
}

-(void) _addOperation:(gap_operation_t)operation
      performSelector:(SEL)selector
             onObject:(id)object
             withData:(id)data
{
    [self addOperationWithBlock:^(void) {
        int result = operation();
        [object performSelectorOnMainThread:selector
                                 withObject:[[IAGapOperationResult alloc] initWithStatusCode:result andData:data]
                              waitUntilDone:NO];
    }];
}


@end

//- User status notification implementation ---------------------------------

#define SET_CSTR(__name)                                                    \
    self.__name = [[NSString alloc] initWithUTF8String:n->__name];          \
/**/

@implementation IAUserStatusNotification

@synthesize user_id;
@synthesize status;

- (id) init:(gap_UserStatusNotification const*)n
{
    self = [super init];
    if (self)
    {
        SET_CSTR(user_id);
        self.status = n->status;
    }
    return self;
}

@end

//- Transaction notification implementation -----------------------------------

@implementation IATransactionNotification

@synthesize first_filename;
@synthesize files_count;
@synthesize total_size;
@synthesize is_directory;
@synthesize network_id;
@synthesize sender_id;
@synthesize sender_fullname;
@synthesize transaction_id;
@synthesize recipient_fullname;

- (id) init:(gap_TransactionNotification const*)n
{
    self = [super init];
    if (self)
    {        
        SET_CSTR(first_filename);
        self.files_count = n->files_count;
        self.total_size = n->total_size;
        self.is_directory = n->is_directory;
        SET_CSTR(network_id);
        SET_CSTR(sender_id);
        SET_CSTR(sender_fullname);
        SET_CSTR(transaction_id);
        SET_CSTR(recipient_fullname);
    }
    return self;
}

@end

//- Transaction status notification implementation ---------------------------

@implementation IATransactionStatusNotification

@synthesize transaction_id;
@synthesize network_id;
@synthesize sender_device_id;
@synthesize recipient_device_id;
@synthesize recipient_device_name;
@synthesize status;

- (id) init:(gap_TransactionStatusNotification const*)n
{
    self = [super init];
    if (self)
    {
        SET_CSTR(transaction_id);
        SET_CSTR(network_id);
        SET_CSTR(sender_device_id);
        SET_CSTR(recipient_device_id);
        SET_CSTR(recipient_device_name);
        self.status = n->status;
    }
    return self;
}

@end

//- notif callback implementation -----------------------------------------------------------

static void on_user_status(gap_UserStatusNotification const* n)
{
    assert(n != NULL);
    NSLog(@">>> on user status notif !");
    [[NSNotificationCenter defaultCenter] postNotificationName:IA_GAP_EVENT_USER_STATUS_NOTIFICATION
                                                        object:[[IAUserStatusNotification alloc] init:n]];
}

static void on_transaction(gap_TransactionNotification const* n)
{
    assert(n != NULL);
    NSLog(@">>> On transaction notif");
    [[NSNotificationCenter defaultCenter] postNotificationName:IA_GAP_EVENT_TRANSACTION_NOTIFICATION
                                                        object:[[IATransactionNotification alloc] init:n]];
}

static void on_transaction_status(gap_TransactionStatusNotification const* n)
{
    assert(n != NULL);
    NSLog(@">>> On transaction status notif");
    [[NSNotificationCenter defaultCenter] postNotificationName:IA_GAP_EVENT_TRANSACTION_STATUS_NOTIFICATION
                                                        object:[[IATransactionStatusNotification alloc] init:n]];
}
