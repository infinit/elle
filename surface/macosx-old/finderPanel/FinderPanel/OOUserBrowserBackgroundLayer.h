#import <Quartz/Quartz.h>
#import <Cocoa/Cocoa.h>


@interface OOUserBrowserBackgroundLayer : CALayer {
	IKImageBrowserView *owner;
}
@property IKImageBrowserView *owner;

@end
