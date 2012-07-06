#import <Quartz/Quartz.h>
#import <Cocoa/Cocoa.h>


@interface OONetworkBrowserBackgroundLayer : CALayer {
	IKImageBrowserView *owner;
}
@property IKImageBrowserView *owner;

@end
