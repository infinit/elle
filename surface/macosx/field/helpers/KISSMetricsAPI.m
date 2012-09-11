//
//  KISSMetricsAPI.m
//  KISSMetricsAPI
//
//  Created by Einar Vollset on 9/15/11.
//  Copyright 2011 KISSMetrics. All rights reserved.
//

#import "KISSMetricsAPI.h"


//When debug, log warnings, else not.
#ifdef DEBUG
#    define InfoLog(...) NSLog(__VA_ARGS__)
#else
#    define InfoLog(...) /* */
#endif


#define BASE_URL @"https://trk.kissmetrics.com"
#define EVENT_PATH @"/e"
#define PROP_PATH @"/s"
#define ALIAS_PATH @"/a"
#define RETRY_INTERVAL 5
#define PROPS_KEY @"DeviceProperties"
#define MAC_SYSTEM_NAME @"Mac OS X"

#define FILE_PATH [[NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"KISSMetrics.actions"]


#define IDENTITY_PATH [[NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"KISSMetrics.identity"]



static KISSMetricsAPI *sharedAPI = nil;


@interface KISSMetricsAPI() 

@property (nonatomic, retain) NSMutableArray *sendQueue;
@property (nonatomic, retain) NSTimer *timer;
@property (nonatomic, retain) NSURLConnection *existingConnection;
@property (nonatomic, retain) NSString *key;
@property (nonatomic, retain) NSString *lastIdentity;
@property (nonatomic, retain) NSDictionary *propsToSend;

-(void) initializeAPIWithKey:(NSString *)apiKey;
-(void) send;

//Saving/unpersisting data
-(void) unarchiveData;
-(void) archiveData;

- (NSString *)urlEncode:(NSString *)prior;
- (NSString *)urlizeProps:(NSDictionary *)props;

//App lifecycle callbacks.
-(void)applicationWillTerminate:(NSNotification *)notification;
-(void)applicationWillEnterForeground:(NSNotificationCenter *)notification;

@end



@implementation KISSMetricsAPI


@synthesize sendQueue;
@synthesize timer;
@synthesize existingConnection;
@synthesize key;
@synthesize lastIdentity;
@synthesize propsToSend;

#pragma mark -
#pragma mark Singleton methods

+ (KISSMetricsAPI *) sharedAPIWithKey:(NSString *)apiKey
{
    @synchronized(self)
    {
        if (sharedAPI == nil) {
            sharedAPI = [[KISSMetricsAPI alloc] init];
            [sharedAPI initializeAPIWithKey:apiKey];
        }
            
    }
    return sharedAPI;
}

+ (KISSMetricsAPI *) sharedAPI
{
    @synchronized(self)
    {
        if (sharedAPI == nil)
        {
            InfoLog(@"KISSMetricsAPI: WARNING - Returning nil object in sharedAPI as sharedAPIWithKey: has not been called.");
        }
    }
    return sharedAPI;
}

- (void) initializeAPIWithKey:(NSString *)apiKey
{
    self.key = apiKey;
    self.lastIdentity = [NSKeyedUnarchiver unarchiveObjectWithFile:IDENTITY_PATH];
    if(!self.lastIdentity) //If there's no identity, generate a UUID as a temp.
    {
        CFUUIDRef theUUID = CFUUIDCreate(NULL);
        CFStringRef string = CFUUIDCreateString(NULL, theUUID);
        CFRelease(theUUID);
        self.lastIdentity = [(NSString *)string autorelease];
        if (![NSKeyedArchiver archiveRootObject:self.lastIdentity toFile:IDENTITY_PATH]) 
        {
            InfoLog(@"KISSMetricsAPI: WARNING - Unable to archive identity!!!");
        }
    }
    
    
    NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];
    //If iOS > 4.0, then we need to listen for the didEnterForeground notification so we can start sending after a suspend.
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 40000        
    if ([[UIDevice currentDevice] respondsToSelector:@selector(isMultitaskingSupported)]) 
    {
        if (&UIApplicationWillEnterForegroundNotification) {
            [notificationCenter addObserver:self 
                                   selector:@selector(applicationWillEnterForeground:) 
                                       name:UIApplicationWillEnterForegroundNotification 
                                     object:nil];
        }
    }
#endif
    //Always listen to will terminate.
    
#if !TARGET_OS_IPHONE
    [notificationCenter addObserver:self 
                           selector:@selector(applicationWillTerminate:) 
                               name:NSApplicationWillTerminateNotification 
                             object:nil];
#else
    [notificationCenter addObserver:self 
                           selector:@selector(applicationWillTerminate:) 
                               name:UIApplicationWillTerminateNotification 
                             object:nil];
#endif
    
    //Check if we've sent or updated the basic properties for this device. 
    BOOL shouldSendProps = YES;
    self.propsToSend = [[NSUserDefaults standardUserDefaults] objectForKey:PROPS_KEY];
    if (self.propsToSend != nil) 
    {
        shouldSendProps = NO;
        
#if TARGET_OS_IPHONE
        if(![[[UIDevice currentDevice] systemName] isEqualToString:[self.propsToSend objectForKey:@"systemName"]])
        {
            shouldSendProps = YES; 
        }
        else if(![[[UIDevice currentDevice] systemVersion] isEqualToString:[self.propsToSend objectForKey:@"systemVersion"]])
        {
            shouldSendProps = YES; 
        }
#else
        if(![MAC_SYSTEM_NAME isEqualToString:[self.propsToSend objectForKey:@"systemName"]])
        {
            shouldSendProps = YES; 
        }
        else if(![[KISSMetricsAPI macVersionNumber] isEqualToString:[self.propsToSend objectForKey:@"systemVersion"]])
        {
            shouldSendProps = YES; 
        }
#endif
        
    }
    
    if(shouldSendProps)
    {
        
#if TARGET_OS_IPHONE
        self.propsToSend = [NSDictionary dictionaryWithObjectsAndKeys:[[UIDevice currentDevice] systemName], @"systemName", [[UIDevice currentDevice] systemVersion], @"systemVersion", nil];
#else
        self.propsToSend = [NSDictionary dictionaryWithObjectsAndKeys:MAC_SYSTEM_NAME, @"systemName", [KISSMetricsAPI macVersionNumber], @"systemVersion", nil];
#endif        
        
        [[NSUserDefaults standardUserDefaults] setObject:self.propsToSend forKey:PROPS_KEY];
        [[NSUserDefaults standardUserDefaults] synchronize];
        
    }
    else
    {
        self.propsToSend = nil; 
    }
    //Note: can't send yet, will do so when entering foreground.
    
    
    [self applicationWillEnterForeground:nil];
}

+ (id)allocWithZone:(NSZone *)zone 
{
    @synchronized(self) {
        if (sharedAPI == nil) 
        {
            sharedAPI = [super allocWithZone:zone];
            return sharedAPI;  // assignment and return on first allocation
        }
    }
    return nil; // on subsequent allocation attempts return nil
}

- (id)copyWithZone:(NSZone *)zone
{
    return self;
}

- (id)retain {
    return self;
}

- (NSUInteger)retainCount {
    return UINT_MAX;  // denotes an object that cannot be released
}

- (oneway void)release {
    //do nothing
}

- (id)autorelease {
    return self;
}


- (id)init
{
    self = [super init];
    if (self) {
    }
    
    return self;
}


#pragma mark -
#pragma mark Lifecycle methods

- (void)applicationWillTerminate:(NSNotification*) notification
{
    @synchronized(self)
    {
        //If timer is != nil, then we cancel it.
        if(self.timer != nil)
        {
            [self.timer invalidate];
            self.timer = nil;
        }
        
        //If existing connection is != nil, cancel it. This will stop any further callbacks.
        if(self.existingConnection != nil)
        {
            [self.existingConnection cancel];
        }
        
        [self archiveData]; //Not 100% sure this is needed, but can't hurt I guess.
    }
}

- (void)applicationWillEnterForeground:(NSNotificationCenter*) notification
{
    @synchronized(self)
    {
        if(self.key != nil)
        {
            [self unarchiveData];
            if(self.propsToSend != nil)
            {
                [self setProperties:self.propsToSend];
                self.propsToSend = nil;
            }
            else
            {
                [self send];
            }
        }
    }
}




//Actually sends the next API call.
- (void) send
{
    
    
    @synchronized(self)
    {
        //If timer is != nil, then we cancel it.
        if(self.timer != nil)
        {
            [self.timer invalidate];
            self.timer = nil;
        }
        
        
        //Ignore this if we have an ongoing connection, as once it completes (successfully or not), it will proceed to the next one.
        if(self.existingConnection != nil)
        {
            return;
        }
        
        NSString *nextAPICall = nil;
        
        //If nothing to do return.
        if([self.sendQueue count] == 0)
        {
            return;
        }
        
        nextAPICall = [self.sendQueue objectAtIndex:0];
        
#if TARGET_OS_IPHONE
        //Networking code.
        [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];
#endif
        
        NSURL *url = [NSURL URLWithString:nextAPICall];
        NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:url];
        
        self.existingConnection = [NSURLConnection connectionWithRequest:request delegate:self];
        [self.existingConnection start];
        [request release];
        
        // If called from a background thread
        if(![NSThread isMainThread]){
            
            // Keep the thread alive until the request completes or fails
            while(self.existingConnection) {
                [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
            }
        }
        
    }
    
}

#pragma mark -
#pragma mark NSURLConnection Callbacks
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSHTTPURLResponse *)response 
{
    if ([response statusCode] == 200 || [response statusCode] == 304) 
    {
        
        //Got HTTP 200 or HTTP 304, which means we can remove the top most API call from the queue.
        @synchronized(self)
        {
            [self.sendQueue removeObjectAtIndex:0];
            [self archiveData];
        }
    } 
    else 
    {
        InfoLog(@"KISSMetricsAPI: INFO - Failure %@", [NSHTTPURLResponse localizedStringForStatusCode:[response statusCode]]);
    }
}




- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error 
{
    
#if TARGET_OS_IPHONE
    [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];
#endif
    
    
    if(error.code == NSURLErrorBadURL ||
       error.code == NSURLErrorUnsupportedURL ||
       error.code == NSURLErrorDataLengthExceedsMaximum)
    {
        //This is v. bad, as it means we've not correctly encoded the URL, that somehow the URL scheme is messed up or that the dev has made too long a URL. Should never/rarely happen, but nice to tell the dev if it does..
        @synchronized(self)
        {
            InfoLog(@"KISSMetricsAPI: CATASTROPHIC FAILURE (%@) for URL (%@). Dropping call..",[error localizedDescription], [self.sendQueue objectAtIndex:0]);
            [self.sendQueue removeObjectAtIndex:0]; // <- We're dropping data..
            [self archiveData];
        }
    }
    
    
    
    @synchronized(self)
    {
        self.existingConnection = nil;
        
        //Effectively cycle through the URLs so as to not have one messed up URL block all others.
        if ([self.sendQueue count] > 1)
        {
            NSString *failedURL = [self.sendQueue objectAtIndex:0];
            [failedURL retain];
            [self.sendQueue removeObjectAtIndex:0];
            [self.sendQueue addObject:failedURL];
            [failedURL release];
        }
        [self archiveData];
        
        
        if(self.timer == nil)//Only if there's no other timer do we schedule a retry.
        {
            self.timer = [NSTimer scheduledTimerWithTimeInterval:RETRY_INTERVAL 
                                                          target:self 
                                                        selector:@selector(send) 
                                                        userInfo:nil 
                                                         repeats:NO];
        }
    }
    
    
}
- (void)connectionDidFinishLoading:(NSURLConnection *)connection 
{
    
    @synchronized(self)
    {
        self.existingConnection = nil;
    }
    
#if TARGET_OS_IPHONE
    [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];
#endif
    
    //Now can try the next one, as this one was successful (might not be one, but that's okay).
    [self send];
}




#pragma mark -
#pragma archival methods

- (void)unarchiveData 
{
    //Not @synch'ed as should always be called inside @sync bloc
    self.sendQueue = [NSKeyedUnarchiver unarchiveObjectWithFile:FILE_PATH];
    if (!self.sendQueue) 
    {
        self.sendQueue = [NSMutableArray array];
    }
    
}
- (void)archiveData 
{
    //Not @synch'ed as should always be called inside @sync bloc
    if (![NSKeyedArchiver archiveRootObject:self.sendQueue toFile:FILE_PATH]) 
    {
        InfoLog(@"KISSMetricsAPI: WARNING - Unable to archive data!!!");
    }
}



#pragma mark -
#pragma utility methods

//Create appropriate url
- (NSString *)urlizeProps:(NSDictionary *)props
{
    NSMutableString *propsURLPart = [NSMutableString string];
    
    for(id propKey in [props allKeys])
    {
        if (![propKey isKindOfClass:[NSString class]]) 
        {
            InfoLog(@"KISSMetricsAPI: WARNING - property keys must be NSString. Dropping property.");
            continue;
        }
        NSString *stringKey = (NSString *)propKey;
        
        
        if([stringKey length] == 0)
        {
            InfoLog(@"KISSMetricsAPI: WARNING - property keys must not be empty strings. Dropping property.");
            continue;
        }
        
        
        
        NSString *stringValue = nil;
        if([props objectForKey:stringKey] == nil)
        {
            InfoLog(@"KISSMetricsAPI: WARNING - property value cannot be nil. Dropping property.");
            continue;
        }
        else if([[props objectForKey:stringKey] isKindOfClass:[NSNumber class]])
        {
            NSNumber *numberValue = (NSNumber *)[props objectForKey:stringKey];
            stringValue = [numberValue stringValue];
        }
        else if([[props objectForKey:stringKey] isKindOfClass:[NSString class]])
        {
            stringValue = (NSString *)[props objectForKey:stringKey];
        }
        
        //If it's not NSNumber of NSString, we drop it.
        if(stringValue == nil)
        {
            InfoLog(@"KISSMetricsAPI: WARNING - property value cannot be of type %@. Dropping property.", [[[props objectForKey:stringKey] class] description]);
            continue;
        }
        
        
        if([stringValue length] == 0)
        {
            InfoLog(@"KISSMetricsAPI: WARNING - property values must not be empty strings. Dropping property.");
            continue;
        }
        
        
        //Check for name length after URL encoding.
        NSString *escapedKey = [self urlEncode:stringKey];
        if([escapedKey length] > 255)
        {
            InfoLog(@"KISSMetricsAPI: WARNING - property key cannot longer than 255 characters. When URL escaped, your key is %lu characters long (the submitted value is %@, the URL escaped value is %@). Dropping property.", [escapedKey length], stringKey, escapedKey);
            continue;
        }
        
        NSString *escapedValue = [self urlEncode:stringValue];
        
        
        //Could check for value & total URL sizes here, but was told not to worry
        [propsURLPart appendFormat:@"&%@=%@", escapedKey, escapedValue];
        
    }
    
    return propsURLPart;
}


//the NSString method doesn't work right, so..
- (NSString *)urlEncode:(NSString *)prior
{
    NSString * after = (NSString *)CFURLCreateStringByAddingPercentEscapes( NULL,(CFStringRef)prior, NULL,(CFStringRef)@"!*'();:@&=+$,/?%#[]", kCFStringEncodingUTF8 );
    return [after autorelease];
    
}



#pragma mark -
#pragma api methods

//Record event.
- (void)recordEvent:(NSString *)name withProperties:(NSDictionary *)properties
{
    
    //Make sure name is good
    if(name == nil || [name length] == 0)
    {
        InfoLog(@"KISSMetricsAPI: WARNING - Tried to record event with empty or nil name. Ignoring.");
        return;
    }
    
    //Must URL escape names.
    NSString *escapedEventName = [self urlEncode:name];
    
    //Need to url escape identity, as could have spaces, etc. lastIdentiy will always be non-nil at this point.
    NSString *escapedIdentity = [self urlEncode:self.lastIdentity];
    
    //We'll store and use the actual time of the record event in case of disconnected operation.
    int actualTimeOfevent = (int)[[NSDate date] timeIntervalSince1970];
    
    NSString *theURL = [NSString stringWithFormat:@"%@%@?_k=%@&_p=%@&_d=1&_t=%i&_n=%@", BASE_URL, EVENT_PATH, self.key, escapedIdentity,actualTimeOfevent,escapedEventName];
    
    if(properties != nil)
    {
        NSString *additionalURL = [self urlizeProps:properties];
        if([additionalURL length] > 0)
        {
            theURL = [NSString stringWithFormat:@"%@%@", theURL,additionalURL];
        }
    }
    
    
    @synchronized(self)
    {
        //Queue up call.
        [self.sendQueue addObject:theURL];
        //Persist the new queue
        [self archiveData];
    }
    //Push it out right not if possible.
    [self send];
}


- (void)setProperties:(NSDictionary *)properties
{
    
    if(properties == nil || [properties count] == 0)
    {
        InfoLog(@"KISSMetricsAPI: WARNING - Tried to set properties with no properties in it..");
        return;
    }
    
    NSString *additionalURL = [self urlizeProps:properties];
    if([additionalURL length] == 0)
    {
        InfoLog(@"KISSMetricsAPI: WARNING - no valid properties in setProperties:. Ignoring call");
        return;
    }
    
    //Need to url escape identity, as could have spaces, etc. lastIdentiy will always be non-nil at this point.
    NSString *escapedIdentity = [self urlEncode:self.lastIdentity];
    
    //We'll store and use the actual time of the record event in case of disconnected operation.
    int actualTimeOfevent = (int)[[NSDate date] timeIntervalSince1970];
    
    NSString *theURL = [NSString stringWithFormat:@"%@%@?_k=%@&_p=%@&_d=1&_t=%i&", BASE_URL, PROP_PATH, self.key, escapedIdentity,actualTimeOfevent];
    
    theURL = [NSString stringWithFormat:@"%@%@", theURL,additionalURL];
    
    
    @synchronized(self)
    {
        //Queue up call.
        [self.sendQueue addObject:theURL];
        //Persist the new queue
        [self archiveData];
    }
    //Push it out right not if possible.
    [self send];
}



- (void)identify:(NSString *)identity
{
    
    if(identity == nil || [identity length] == 0)
    {
        InfoLog(@"KISSMetricsAPI: WARNING - attempted to use nil or empty identity. Ignoring.");
        return;
    }
    
    
    NSString *escapedOldIdentity = [self urlEncode:self.lastIdentity];
    NSString *escapedNewIdentity = [self urlEncode:identity];
    
    
    NSString *theURL = [NSString stringWithFormat:@"%@%@?_k=%@&_p=%@&_n=%@", BASE_URL, ALIAS_PATH, self.key, escapedOldIdentity,escapedNewIdentity];
    
    
    
    
    @synchronized(self)
    {
        //Now we must update the identity on disk. No need to wait until the alias has been 
        //accepted on the server, as we calls are FIFO and encoded with current identity.
        self.lastIdentity = identity;
        
        if (![NSKeyedArchiver archiveRootObject:self.lastIdentity toFile:IDENTITY_PATH]) 
        {
            InfoLog(@"KISSMetricsAPI: WARNING - Unable to archive new identity!!!");
        }
        
        [self.sendQueue addObject:theURL];
        //Persist the new queue
        [self archiveData];
    }
    //Push it out right not if possible.
    [self send];
    
    
    
}


- (void)alias:(NSString *)firstIdentity withIdentity:(NSString *)secondIdentity
{
    if(firstIdentity == nil || [firstIdentity length] == 0 || secondIdentity == nil || [secondIdentity length] == 0)
    {
        InfoLog(@"KISSMetricsAPI: WARNING - attempted to use nil or empty identities in alias (%@ and %@). Ignoring.", firstIdentity, secondIdentity);
        return;
    }
    
    NSString *escapedFirstIdentity = [self urlEncode:firstIdentity];
    NSString *escapedSecondIdentity = [self urlEncode:secondIdentity];
    
    
    NSString *theURL = [NSString stringWithFormat:@"%@%@?_k=%@&_p=%@&_n=%@", BASE_URL, ALIAS_PATH, self.key, escapedFirstIdentity,escapedSecondIdentity];
    
    
    @synchronized(self)
    {
        [self.sendQueue addObject:theURL];
        [self archiveData];
    }
    //Push it out right not if possible.
    [self send];
    
}


#if !TARGET_OS_IPHONE
+ (NSString *)macVersionNumber
{
    OSErr err;
    SInt32 major, minor, bugfix;
    err = Gestalt(gestaltSystemVersionMajor, &major);
    if (err != noErr) return nil;
    err = Gestalt(gestaltSystemVersionMinor, &minor);
    if (err != noErr) return nil;
    err =Gestalt(gestaltSystemVersionBugFix, &bugfix);
    if (err != noErr) return nil;
    
    return [NSString stringWithFormat:@"%d.%d.%d", major, minor, bugfix];
}
#endif

@end
