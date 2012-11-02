//
//  IAGapState.m
//  FinderWindowApplication
//
//  Created by infinit on 10/29/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAGapState.h"

#import "gap.h"


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
        NSLog(@"Timer fired !");
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

// Block type to queue gap operation
typedef int(^gap_operation_t)(void);

@interface IAGapOperationResult ()
{
    gap_Status status;
}

@property gap_Status status;

- (id)initWithStatusCode:(gap_Status)status_code;

@end


@implementation IAGapOperationResult

@synthesize status;

- (id)initWithStatusCode:(gap_Status)status_code
{
    self.status = status_code;
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







static void on_user_status(gap_UserStatusNotification const* n);


@interface IAGapState ()
{
    gap_State* _state;
    BOOL _logged_in;
    BOOL _polling;
}


@property gap_State* state;

@end

@implementation IAGapState

@synthesize state = _state;
@synthesize logged_in = _logged_in;

+ (IAGapState*) instance
{
    static IAGapState* instance = NULL;

    if (instance == NULL)
        instance = [[IAGapState alloc] init];
    if (instance == NULL)
        [NSException raise:@"bad_alloc" format:@"Cannot create a new gap state"];
    return instance;
}

-(id) init
{
    self = [super init];
    
    if (!self)
        return nil;
    
    _state = gap_new();
    if (_state == NULL)
        [NSException raise:@"bad_alloc" format:@"Cannot create a new gap state"];
    _logged_in = FALSE;
    _polling = FALSE;
    return self;
}

// Wrapper that is only called from _startPolling or a timer
- (void)_poll
{
    if (!_polling || !_logged_in)
        return;
    NSLog(@"Do poll");
    gap_Status ret = gap_poll(_state);
    [self addOperation:[[TimerOperation alloc] initWithInterval:1
                                                performSelector:@selector(_poll)
                                                       onObject:self]];
}

// Start polling trophonius
- (void)_startPolling
{
    @synchronized(self)
    {
        if (!_polling && _logged_in)
        {
            _polling = TRUE;
            NSLog(@"Start polling");
            [self _poll];
        }
    }
}

//- User -------------------------------------------------------------------------------------



- (void)                login:(NSString*)login
                 withPassword:(NSString*)password
                andDeviceName:(NSString*)device_name
              performSelector:(SEL)selector
                     onObject:(id)object;
{
    __weak id this = self;
    [self _addOperation:^(void) {
        char* hash_password = gap_hash_password(self.state,
                                                [login UTF8String],
                                                [password UTF8String]);
        int res = gap_login(self.state,
                            [login UTF8String],
                            hash_password);
        NSLog(@"Pass: %s", hash_password);
        gap_hash_free(hash_password);
        if (res == gap_ok)
            res = gap_set_device_name(self.state, [device_name UTF8String]);
        
        if (res == gap_ok)
            res = gap_trophonius_connect(self.state);
        else
            NSLog(@"Cannot login !");
        
        if (res == gap_ok)
            res = gap_user_status_callback(self.state, &on_user_status);
        else
            NSLog(@"Cannot connect to tropho");
        
        if (res == gap_ok)
        {
            self.logged_in = TRUE;
            [this _startPolling];
        }
        else
            NSLog(@"Cannot register callback");
        
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
    __weak id this = self;
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
            [this _startPolling];
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

@end

static void on_user_status(gap_UserStatusNotification const* n)
{
    NSLog(@"CDSJKFADSJKL");
}
