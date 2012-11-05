//
//  OVPMediaPlayerController.m
//  OVPMediaPlayerController
//
// This file is part of the OpenVideoPlayer project, http://openvideoplayer.com
// Copyright © 2008-2010, Akamai Technologies.  All rights reserved.
// Originally created by Petrovic, Tommy (tpetrovi@akamai.com)
// OpenVideoPlayer is free software, you may use, modify, and/or redistribute under the terms of the license:
// http://openvideoplayer.com/license
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.

#import <MediaPlayer/MediaPlayer.h>
#import <Availability.h>
#import "OVPMediaPlayerController.h"

#import <objc/objc.h>
#import <objc/runtime.h>
#import <objc/message.h>


NSString * const OVPMovieWillPlayNotification = @"OVPMovieWillPlayNotification";
static NSString *PlayNextMediaNotification = @"OVPPlayNextMediaInQueue";



#if !defined( __IPHONE_OS_VERSION_MIN_REQUIRED) || __IPHONE_OS_VERSION_MIN_REQUIRED < 30200

static OVPMoviePlayerController* _currentPlayer = nil; // this reference is used to process multi-plays (queue processing) in 3.0/3.1
static UIView *_behindPlayerView = nil; // global view used to smooth over the player transitions between plays, mostly used when playlisting/queueing
BOOL _statusBarHidden; // used to restore status bar to its original visibility state

NSObject *_lockObject = nil;
int _notificationCountPerPlay = 0;

// SDK 3.0/3.1 Only...

// The following category extends UIView to add method(s) used to manipulate adding of the overlays on top of the
// MPMoviePlayerController window. The way we do it here is to extend UIView to add a listener to the Player's
// pre-load finished notification and then during the notification processing, add the view to the Player's window as a subview.
@interface UIView (MoviePlayerExtension)
- (void)processMoviePreloadFinishNotification:(NSNotification*)notification;
@end
@implementation UIView (MoviePlayerExtension)

- (void)processMoviePreloadFinishNotification:(NSNotification*)notification
{
	UIApplication *theApp = [UIApplication sharedApplication];
	if ([[theApp windows] count]>1)
	{
		UIWindow *playerWindow = [theApp keyWindow];
		if (playerWindow)
			[playerWindow addSubview:self];
	}

	[[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:MPMoviePlayerContentPreloadDidFinishNotification
                                                  object:nil];

}

@end // UIView (MoviePlayerExtension) category
#endif //  #if !defined( __IPHONE_OS_VERSION_MIN_REQUIRED) || __IPHONE_OS_VERSION_MIN_REQUIRED < 30200

// Some internal implementations for the support of the play queue
@interface OVPMoviePlayerController (OVPInternal)
#if defined( __IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED >= 30200
-(OVPStatus)playQueueInternal;
#else
// Note that In 3.0/3.1 Impl, playQueueInternal is defined as a static method
+(OVPStatus)playQueueInternal:(id)playerInstance;
#endif

@end
@implementation OVPMoviePlayerController (OVPInternal)


#if defined( __IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED >= 30200
// 3.2+ SDK Implementation.
// This function governs the queue play execution
#pragma mark - 3.2+ Implementation
-(OVPStatus)playQueueInternal
{

	id dequeuedItem = nil;
	BOOL usingOVPMediaInitObject = NO;
	@synchronized (self)
	{
		// dequeue the media url
		if (mediaQueue_==nil || [mediaQueue_ count]==0)
			return OVP_NO_MEDIA_ENQUEUED;

		dequeuedItem = /*[*/[mediaQueue_ objectAtIndex:0]/* retain]*/;
		[mediaQueue_ removeObjectAtIndex:0]; // remove the media item
	}
	usingOVPMediaInitObject = ([dequeuedItem conformsToProtocol:@protocol(OVPMediaInitialize)]);
	if (!usingOVPMediaInitObject && ![dequeuedItem isKindOfClass:[NSURL class]])
		return OVP_MEDIA_ITEM_OF_UNSUPPORTED_TYPE;

	if(usingOVPMediaInitObject)
	{
		if ( [dequeuedItem respondsToSelector:@selector(setPlayerPlaybackSettings:)] )
		{
			[dequeuedItem setPlayerPlaybackSettings:self];
		}

	}
	// If enqueued item adopts OVPMediaInitialize protocol, call its mediaURL call to extract the URL at this point
	NSURL *contentURL = (usingOVPMediaInitObject?[dequeuedItem mediaURL]:dequeuedItem);
	[self setContentURL:contentURL];
	NSLog(@"playQueue media url=%@",[contentURL absoluteString]);
//	[dequeuedItem release];
	[self pause]; // pause/stop combo due to Apple's bug per their dev site
	[self stop];
	[self play];
	return OVP_NO_ERROR;
}


#else
#pragma mark - 3.0/3.1 Implementation
// SDK 3.0/3.1 support, notice this is different from 3.2+ version in that it's implemented using a static method
+(OVPStatus)playQueueInternal:(id)playerInstance
{
	NSLog(@"player instance = %X", playerInstance);

	id dequeuedItem = nil;
	BOOL usingOVPMediaInitObject = NO;
	NSLog(@"inside playQueueINternal");
	// dequeue the media url
	OVPMoviePlayerController *mpc = playerInstance;
	@synchronized (mpc)
	{
		if (mpc.mediaQueue_==nil || [mpc.mediaQueue_ count]==0)
			return OVP_NO_MEDIA_ENQUEUED;

		dequeuedItem = [[mpc.mediaQueue_ objectAtIndex:0] retain];
		[mpc.mediaQueue_ removeObjectAtIndex:0]; // remove the media item
	}
	usingOVPMediaInitObject = ([dequeuedItem conformsToProtocol:@protocol(OVPMediaInitialize)]);
	if (!usingOVPMediaInitObject && ![dequeuedItem isKindOfClass:[NSURL class]])
		return OVP_MEDIA_ITEM_OF_UNSUPPORTED_TYPE;

	// If enqueued item adopts OVPMediaInitialize protocol, call its mediaURL call to extract the URL at this point
	NSURL *contentURL = (usingOVPMediaInitObject?[dequeuedItem mediaURL]:dequeuedItem);
	NSLog(@"playQueueInternal media url=%@",[contentURL absoluteString]);

	OVPMoviePlayerController *ovpMovieController =  [[OVPMoviePlayerController alloc] initWithContentURL:contentURL]; // store a new instance of OVPMoviePlayerController internally

	[mpc.internal_ release];
	mpc.internal_ = ovpMovieController;
	[ovpMovieController release];

	if (!mpc.internal_)
	{
		[dequeuedItem release];
		return OVP_PLAYER_FAILED_TO_INITIALIZE;
	}

	if(usingOVPMediaInitObject)
	{
		if ( [dequeuedItem respondsToSelector:@selector(setPlayerPlaybackSettings:)] )
		{
			[dequeuedItem setPlayerPlaybackSettings:mpc.internal_];
		}
		if ([dequeuedItem respondsToSelector:@selector(getOverlayView)])
		{
			// It is the receiver's responsibility to take care of the lifecycle of the views added as a subview to this player
			// Overlay views could be different for each play, hence we process this via a delegate method getOverlayView
			UIView *overlayView = [dequeuedItem getOverlayView];
			// Notice if nil is returned by the call we do nothing... not an error, not an exception, simply no overlays will be displayed
			// replace any existing overlay view with the new one...
			if (mpc.overlayView_ && [mpc.overlayView_ superview])
			{
				[mpc.overlayView_ removeFromSuperview];  //remove the existing/old view
			}
			mpc.overlayView_ = overlayView; // 'replace' internal overlay view to relay forward, if nil no view is added
			// attach this view to the player
			if (overlayView)
				[mpc.internal_ addSubview:overlayView];

		}


	}
	else
	{
		if (mpc.overlayView_)
		{
			NSLog(@"overlay view will be set...");
			[mpc.internal_ addSubview:mpc.overlayView_]; // the view is added regardless of the player instance, the view will be added on the next play
		}
	}


	// before playing, execute any delegate methods that have been registered on this object
	if (mpc.object)
	{
		[mpc.object movieWillPlay:(OVPMoviePlayerController*)mpc.internal_];
	}
	[dequeuedItem release];
	[mpc.internal_ play];
	return OVP_NO_ERROR;
}


#endif // #if defined( __IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED >= 30200
@end


// This internal class is responsible for managing playing of queued up media automatically
@interface OVPPlayerControllerInternal: NSObject


// OVPPlayerControllerInternal class will also serve as a notification delegate for the MPPlayerController that it embeds
// Following two notification handlers are common between 3.0/3.1 and 3.2+ versions
- (void)processMoviePlaybackFinishedNotification:(NSNotification*)notification;
- (void)processPlayNextMediaNotification:(NSNotification*)notification;
#if defined( __IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED >= 30200
- (void)processMoviePlayerStateChangedNotification:(NSNotification*)notification;
- (void)processMoviePlayerWillExitFullScreenNotification:(NSNotification*)notification;
#else
// 3.0/3.1 specific selector(s)
- (void)processMoviePreloadFinishNotification:(NSNotification*)notification;
#endif
- (void)dealloc;
- (OVPPlayerControllerInternal*)init;

+ (void)initialize;
+ (OVPPlayerControllerInternal*)sharedInstance;
@end


static OVPPlayerControllerInternal *_playerControllerInternal = nil;


@implementation OVPPlayerControllerInternal

+ (void)initialize
{
	if(self == [OVPPlayerControllerInternal class])
	{
		_playerControllerInternal = [[OVPPlayerControllerInternal alloc] init];
	}


}

+ (OVPPlayerControllerInternal*)sharedInstance
{
	// Note that we don't have to synchronize this call
	return _playerControllerInternal;
}
#if defined( __IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED >= 30200
// SDK 3.2+ Implementation
#pragma mark - 3.2+ Implementation

// This notification covers the use case where the MPMoviePlayerController was running full-screen, since
// in that case in 3.2 we will not receive MoviePlaybackFinishedNotification.
- (void)processMoviePlayerWillExitFullScreenNotification:(NSNotification*)notification
{
	[self processMoviePlaybackFinishedNotification:notification];
}

- (void)processMoviePlaybackFinishedNotification:(NSNotification*)notification
{
	// initiate next media play asynchronously
	NSNotification *playMediaNotification =
	[NSNotification notificationWithName:PlayNextMediaNotification object:[notification object]];
	[[NSNotificationQueue defaultQueue]
	 enqueueNotification:playMediaNotification
	 postingStyle:NSPostWhenIdle
	 coalesceMask:NSNotificationCoalescingOnName
	 forModes:nil];

}
#else
// SDK 3.0/3.1 Implementation
// processMoviePlaybackFinishedNotification handler is same in 3.2 and 3.0/3.1 for now, however for handling multiple spurious notifications
// from the player, a different logic must be implemented.
#pragma mark - 3.0/3.1 Implementation
- (void)processMoviePlaybackFinishedNotification:(NSNotification*)notification
{
	@synchronized (_lockObject)
	{
		NSLog(@"inisde lock object section");
		_notificationCountPerPlay++;
		NSLog(@"current _notificationCountPerPlay=%d", _notificationCountPerPlay );
		// guarding against multiple spurious notifications
		if (_notificationCountPerPlay > 1) return;

	}
	[self performSelector:@selector(processPlayNextMediaNotification:) withObject:notification afterDelay:3];

}

- (void)processMoviePreloadFinishNotification:(NSNotification*)notification
{
	@synchronized (_lockObject)
	{
		_notificationCountPerPlay = 0;
		NSLog(@"Processing processMoviePreloadFinishNotification, setting _notificationCountPerPlay=%d",_notificationCountPerPlay);
	}
}

#endif

- (void)processPlayNextMediaNotification:(NSNotification*)notification
{
	NSLog(@"Inside processPlayNextMediaNotification");
#if defined( __IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED >= 30200
	// SDK 3.2+ Implementation
	if ([[notification object] isKindOfClass:[OVPMoviePlayerController class]])
	{
		OVPMoviePlayerController *mpc = [notification object];
		if ([mpc playQueueInternal]==OVP_NO_MEDIA_ENQUEUED)
		{
			// make final delegate call if object is registered
			id obj = mpc.object;
			if ([obj respondsToSelector:@selector(queuePlayEnded:)])
				[mpc.object queuePlayEnded:mpc];
		}

	}

#else
	// SDK 3.0/3.1 Implementation
	// We use OVPMoviePlayerController both as a container and as a marker interface, so here we filter those coming from OVP.
	if ([[notification object] isKindOfClass:[OVPMoviePlayerController class]])
	{
		// Here we check if we have an existing player allocated and statically referenced.
		// The difference is that if existing player is used, it means we are processing a queue
		// (3.0/3.1 impl require one url per player instance) so this presents a challenge
		// we will process the next item using the controller which originated the queue playing

		OVPMoviePlayerController *mpc = nil;
		if (_currentPlayer)
		{
			mpc = _currentPlayer;
			NSLog(@"Got _currentPlayer");
		}
		else
			mpc = [notification object]; // this happens when doing single plays
		if ([OVPMoviePlayerController playQueueInternal:mpc]==OVP_NO_MEDIA_ENQUEUED)
		{
			NSLog(@"Removing player view");
			// if finished with the queue processing, 'remove' the transitional background view
			if (_behindPlayerView)
			{
				[_behindPlayerView setAlpha:0.0f];
			}

			UIApplication *app = [UIApplication sharedApplication];
			// restore the status bar
			if (_statusBarHidden)
				[app setStatusBarHidden:YES animated:NO];
			else
				[app setStatusBarHidden:NO animated:NO];

			// restore the status bar to portrait orientation
			[app setStatusBarOrientation:UIInterfaceOrientationPortrait animated:NO];
			// make final delegate call if an object is registered
			id obj = mpc.object;
			if ([obj respondsToSelector:@selector(queuePlayEnded:)])
				[mpc.object queuePlayEnded:mpc];
			// delete (if any) static reference to the OVP object contained in _currentPlayer
			_currentPlayer = nil; // clear any global player references used during queue playing; notice that this is not a 'retained' value

		}
	}
#endif
}

#if defined( __IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED >= 30200
// SDK 3.2+ Implementation
-(OVPPlayerControllerInternal*)init
{
	if (self=[super init])
	{
		NSNotificationCenter *defaultCenter =[NSNotificationCenter defaultCenter];
		[defaultCenter addObserver:self
										 selector:@selector(processMoviePlayerWillExitFullScreenNotification:)
											 name:MPMoviePlayerWillExitFullscreenNotification
										   object:nil];
		[defaultCenter addObserver:self
										 selector:@selector(processMoviePlayerStateChangedNotification:)
											 name:MPMoviePlayerLoadStateDidChangeNotification
										   object:nil];
		[defaultCenter addObserver:self
										 selector:@selector(processMoviePlaybackFinishedNotification:)
											 name:MPMoviePlayerPlaybackDidFinishNotification
										   object:nil];
		[defaultCenter addObserver:self
										 selector:@selector(processPlayNextMediaNotification:)
											 name:PlayNextMediaNotification
										   object:nil];
	}
	return self;
}

-(void)dealloc
{
	NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
	[defaultCenter removeObserver:self name:MPMoviePlayerWillExitFullscreenNotification object:nil];
	[defaultCenter removeObserver:self name:MPMoviePlayerLoadStateDidChangeNotification object:nil];
	[defaultCenter removeObserver:self name:MPMoviePlayerPlaybackDidFinishNotification  object:nil];
	[defaultCenter removeObserver:self name:PlayNextMediaNotification object:nil];

//	[super dealloc];
}
// Only supported in 3.2+ SDK
- (void)processMoviePlayerStateChangedNotification:(NSNotification*)notification
{

	if ([[notification object] isKindOfClass:[OVPMoviePlayerController class]])
	{
		OVPMoviePlayerController *mpc = [notification object];
		if (mpc)
		{
			// before playing, exectute any delegate methods that have been registered with this player
			id playbackDelegateObj = mpc.object;
			MPMovieLoadState loadState = mpc.loadState;
			if (loadState == MPMovieLoadStatePlayable)
				[playbackDelegateObj movieWillPlay:mpc];

		}
	}

}
#else
// SDK 3.0/3.1 Implementation
-(OVPPlayerControllerInternal*)init
{
	if (self=[super init])
	{

		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(processMoviePlaybackFinishedNotification:)
													 name:MPMoviePlayerPlaybackDidFinishNotification
												   object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(processPlayNextMediaNotification:)
													 name:PlayNextMediaNotification
												   object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(processMoviePreloadFinishNotification:)
													 name:MPMoviePlayerContentPreloadDidFinishNotification
												   object:nil];
	}
	return self;
}
-(void)dealloc
{
	NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
	[defaultCenter removeObserver:self
							 name:MPMoviePlayerPlaybackDidFinishNotification
						   object:nil];
	[defaultCenter removeObserver:self
							 name:PlayNextMediaNotification
						   object:nil];
	[defaultCenter removeObserver:self
							 name:MPMoviePlayerContentPreloadDidFinishNotification
						   object:nil];

	[super dealloc];
}


#endif //#if defined( __IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED >= 30200


@end



@implementation OVPMoviePlayerController

@synthesize mediaQueue_, mediaURL;
@synthesize object;
#if !defined( __IPHONE_OS_VERSION_MIN_REQUIRED) || __IPHONE_OS_VERSION_MIN_REQUIRED < 30200
// SDK 3.0/3.1-Specific implementation
@synthesize behindPlayerView_, internal_, overlayView_;
-(void)addSubview:(UIView *)view
{
	// Register to receive a notification to be able to capture player's window more accurately.
	// Upon processing of this notification, we'll add this view to the player's window as a subview.
	self.overlayView_ = view; // store view inside the object to relay forward, this ref is mostly used during queue processing
	[[NSNotificationCenter defaultCenter] addObserver:view
											 selector:@selector(processMoviePreloadFinishNotification:)
												 name:MPMoviePlayerContentPreloadDidFinishNotification
											   object:nil];

}
#endif

-(OVPMoviePlayerController*)init
{
	self = [super init];
	object = nil;
#if !defined( __IPHONE_OS_VERSION_MIN_REQUIRED) || __IPHONE_OS_VERSION_MIN_REQUIRED < 30200
	internal_ = nil;
#endif
	return self;
}

-(NSURL*)modifiedURL:(NSURL*)url
{
#if !defined(OVP_REMOVE_VERSION_FROM_URL)

	NSString *modURLStr = [NSString stringWithFormat:@"%@%@%@=%@",[url absoluteString],([url query]?@"&":@"?"),@_OVPIV,_OVP_INTERNAL_VERSION_STRING];
	return [NSURL URLWithString:modURLStr];
#else
	return nil;
#endif
}

-(id)initWithContentURL:(NSURL *)url
{
	object = nil;
	NSURL *modURL = nil;
#if !defined( __IPHONE_OS_VERSION_MIN_REQUIRED) || __IPHONE_OS_VERSION_MIN_REQUIRED < 30200
	internal_ = nil;
	// modify url to append version
	modURL = [self modifiedURL:url];
#endif
	return [super initWithContentURL:(modURL?modURL:url)];
}


-(OVPMoviePlayerController*)initPlayerQueueWithArray:(NSArray*)array
{

	if (!self.mediaQueue_)
	{
		self.mediaQueue_ = [NSMutableArray arrayWithCapacity:[array count]];
	}
	[self.mediaQueue_ setArray:array];
	id dequeuedItem = /*[*/[self.mediaQueue_ objectAtIndex:0]/* retain]*/;
	[self.mediaQueue_ removeObjectAtIndex:0];
	NSURL *contentURL=nil;
	BOOL usingOVPMediaInitObject = ([dequeuedItem conformsToProtocol:@protocol(OVPMediaInitialize)]);
	if (usingOVPMediaInitObject)
	{
		contentURL = [dequeuedItem mediaURL]; // call the delegate to get the url
	}
	else if ([dequeuedItem isKindOfClass:[NSURL class]])
	{
		contentURL = dequeuedItem;
	}
	object = nil;
#if !defined( __IPHONE_OS_VERSION_MIN_REQUIRED) || __IPHONE_OS_VERSION_MIN_REQUIRED < 30200
	internal_ = nil;
#endif
	// Note that in case array doesn't contain items of NSURL or objects conforming to OVPMediaInitialize
	// a nil will be used to initialize deferring to whatever happens with a call to super when nil is used.
	return [super initWithContentURL:contentURL];
}



-(OVPStatus)enqueueItem:(id)item isFirst:(BOOL)doFirst
{
	if (!item)
		return OVP_URL_IS_NIL; // can't enqueue nil, so intercept and return an error
	BOOL itemIsURL = ([item isKindOfClass:[NSURL class]]);
	if (itemIsURL && ![(NSURL*)item scheme])
		return OVP_URL_MALFORMED;
	if (!(itemIsURL || [item conformsToProtocol:@protocol(OVPMediaInitialize)]))
		return OVP_MEDIA_ITEM_OF_UNSUPPORTED_TYPE;
	@synchronized (self)
	{
		if (!mediaQueue_)
				mediaQueue_ = [[NSMutableArray alloc] init];
			if (doFirst)
				[mediaQueue_ insertObject:item atIndex:0];
			else
				[mediaQueue_ addObject:item];
	}
	return OVP_NO_ERROR;

}

-(void)assignPlayDelegate:(id<OVPMediaPlayback>)obj
{
	self.object = obj;

}



-(OVPStatus)enqueueLastItem:(id)item
{
	return [self enqueueItem:item isFirst:NO];
}

-(OVPStatus)enqueueFirstItem:(id)item
{
	return [self enqueueItem:item isFirst:YES];

}
-(void)play
{

	[[NSNotificationCenter defaultCenter] postNotificationName:OVPMovieWillPlayNotification object:self userInfo:nil];
#if defined( __IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED >= 30200
	// 3.2+ segment
	// url can be modified at this point in 3.2 and later
	NSURL *modURL = [self modifiedURL:[self contentURL]];
	if (modURL)
		[self setContentURL:modURL];
#else // 3.0/3.1 segment
	NSLog(@"play count %d",[mediaQueue_ count]);
	// are we playing the queue?
	if ([mediaQueue_ count]>0)
	{
		_currentPlayer = self; // store this player reference globally as we'll process the queue asynchronously using notifications.
								  // 3.0/3.1 only allows a single player instance
								  // Note: we can't rely on notification object containing the right player instance.


		// if the queue count is non-zero, set up a background view to smooth transitions between plays on 3.0/3.1

		// setup background view for multiple plays as the background will fade in and out during the
		// transitioning of the player's windows and this trick will create seamless transition
		UIApplication *theApp = [UIApplication sharedApplication];
		if (_behindPlayerView)
		{
			[_behindPlayerView setAlpha:1.0f];
		}
		else
		{
			//set up the background view one time
			_behindPlayerView =[[UIView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
			_behindPlayerView.backgroundColor = [UIColor blackColor];
			[[theApp keyWindow] addSubview:_behindPlayerView];
			[_behindPlayerView release];
		}

		if (!theApp.statusBarHidden)
		{
			_statusBarHidden = NO; // temp store the status of the status-bar so we can restore it upon queue finishing
			[theApp setStatusBarHidden:YES animated:NO];
		}
		else
		{
			_statusBarHidden = YES;
		}

	}

#endif
	// for 3.2+ implementation, we simply execute play
	[super play];
}

-(void)dealloc
{
/*	[mediaQueue_ release];
	[mediaURL release];*/
#if !defined( __IPHONE_OS_VERSION_MIN_REQUIRED) || __IPHONE_OS_VERSION_MIN_REQUIRED < 30200
	[internal_ release];
	[overlayView_ release];
#endif
//	[super dealloc];
}

+(void)initializeOVP
{
	[OVPPlayerControllerInternal sharedInstance];
#if !defined( __IPHONE_OS_VERSION_MIN_REQUIRED) || __IPHONE_OS_VERSION_MIN_REQUIRED < 30200
	_lockObject = [[NSObject alloc] init];
#endif
	// Init other libraries...
	// Check for Media Extension Implementation
	NSString *AKAMMediaExtClass = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"MAClassName"];
	NSString *mediaExtURL = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"MAConfigURL"];
	if (AKAMMediaExtClass && mediaExtURL)
	{
		Class mediaExtClass = nil;
		BOOL implAvailable = NO;
		if ((mediaExtClass = NSClassFromString(AKAMMediaExtClass))!=nil &&
			(implAvailable=[mediaExtClass respondsToSelector:@selector(initWithConfigURL:)])==YES)
		{
			NSDictionary *initParameters = [NSDictionary dictionaryWithObjectsAndKeys:mediaExtURL, @"url",
													_OVP_INTERNAL_PLAYER_NAME, @"playerName",
													_OVP_INTERNAL_VERSION_STRING, @"version", nil];

			[mediaExtClass performSelector:@selector(initWithConfigURL:) withObject:initParameters];

		}
	}
}

+(void)deinitializeOVP
{
	// Mainly used to 'terminate' any 3rd party libraries used in conjunction with OVP
	// Check for Media Extension Implementation
	NSString *AKAMMediaExtClass = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"MAClassName"];
	if (AKAMMediaExtClass)
	{
		Class mediaExtClass = nil;
		BOOL implAvailable = NO;
		if ((mediaExtClass = NSClassFromString(AKAMMediaExtClass))!=nil &&
			(implAvailable=[mediaExtClass respondsToSelector:@selector(terminate)])==YES)
		{

			[mediaExtClass performSelector:@selector(terminate)];

		}
	}
}
@end
