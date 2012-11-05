//
//  OVPMediaPlayerController.h
//  OVPMediaPlayerController
//
// This file is part of the OpenVideoPlayer project, http://openvideoplayer.com
// Copyright © 2008-2010, Akamai Technologies.  All rights reserved.
// Originally created by Petrovic, Tommy (tpetrovi@akamai.com)
// OpenVideoPlayer is free software, you may use, modify, and/or redistribute under the terms of the license:
// http://openvideoplayer.com/license
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.

#import <Foundation/Foundation.h>
#import <MediaPlayer/MediaPlayer.h>
#import <UIKit/UIKit.h>

// OVP Version and player name defines
#define _OVP_INTERNAL_VERSION "1.0.0"

#ifdef __IPHONE_OS_VERSION_MIN_REQUIRED
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= 30200
#define _OVP_PLAYER_NAME "OVP_iPhone-iPad"
#else
#define _OVP_PLAYER_NAME "OVP_iPhone"
#endif
#endif
#define _OVPIV "ovpiv"


static NSString *const _OVP_INTERNAL_VERSION_STRING = @_OVP_INTERNAL_VERSION;
static NSString *const _OVP_INTERNAL_PLAYER_NAME = @_OVP_PLAYER_NAME;

extern NSString *const OVPMovieWillPlayNotification;

// Errors returned by the OVP calls
typedef enum 
{
	OVP_NO_ERROR = 0,
	OVP_URL_MALFORMED,
	OVP_URL_IS_NIL,
	OVP_NO_MEDIA_ENQUEUED,
	OVP_PLAYER_FAILED_TO_INITIALIZE,
	OVP_MEDIA_ITEM_OF_UNSUPPORTED_TYPE,
	OVP_PARAMETER_NOT_INITIALIZED,
	OVP_METHOD_NOT_SUPPORTED
}OVPStatus;

//
// Following Protocols are mainly used in handling internal queued media items
// You class should adopt this protocol in order to provide customization on the current MoviePlayer object,
// as well as to provide URL for the clip/movie to be played in the queue.
// When enqueueFirstItem/enqueueLastItem methods mediaURL will be called before 
// MPMoviePlayerController object has been instantiated to get the url of the media to play.
//
@class OVPMoviePlayerController;
@protocol OVPMediaInitialize 
@required
-(NSURL*)mediaURL; 
@optional
-(void)setPlayerPlaybackSettings:(OVPMoviePlayerController*)moviePlayer;
-(UIView *)getOverlayView; // this is provided as a helper call
@end

// Adopt OVPMediaPlayback protocol when interested in receiving movieWillPlay call 
// just before the play and pass it to +assignPlayDelegate: call.  
//
@protocol OVPMediaPlayback 

-(void)movieWillPlay:(OVPMoviePlayerController*)moviePlayer;
@optional
-(void)queuePlayEnded:(OVPMoviePlayerController*)moviePlayer;
@end


@interface OVPMoviePlayerController : MPMoviePlayerController
{
	@private
	NSMutableArray *mediaQueue_;
	id<OVPMediaPlayback> object;
	id mediaURL;
#if !defined( __IPHONE_OS_VERSION_MIN_REQUIRED) || __IPHONE_OS_VERSION_MIN_REQUIRED < 30200

	id internal_;
	UIView *behindPlayerView_;
	UIView *overlayView_;
#endif
}
@property (nonatomic, retain) NSMutableArray *mediaQueue_;
@property (nonatomic, retain) id mediaURL;
@property (nonatomic) id<OVPMediaPlayback> object;
#if !defined( __IPHONE_OS_VERSION_MIN_REQUIRED) || __IPHONE_OS_VERSION_MIN_REQUIRED < 30200
@property (nonatomic, retain) UIView *behindPlayerView_;
@property (nonatomic, retain) id internal_;
@property (nonatomic, retain) UIView *overlayView_;
#endif

+(void)initializeOVP;
+(void)deinitializeOVP;

-(OVPMoviePlayerController*)initPlayerQueueWithArray:(NSArray*)array;
-(OVPStatus)enqueueLastItem:(id)item;
-(OVPStatus)enqueueFirstItem:(id)item;
-(void)assignPlayDelegate:(id<OVPMediaPlayback>)object;
-(void)play; 
#if !defined( __IPHONE_OS_VERSION_MIN_REQUIRED) || __IPHONE_OS_VERSION_MIN_REQUIRED < 30200
-(void)addSubview:(UIView *)view;
#endif

@end




