#import "OOBrowserUtilities.h"


//---------------------------------------------------------------------------------
// createImageWithName:
//
// simple utility function that create an image from a name by looking into the main bundle
//---------------------------------------------------------------------------------
CGImageRef createImageWithName(NSString * imageName)
{
	CGImageRef returnValue = NULL;
	
	NSString *path = [[NSBundle mainBundle] pathForResource:[imageName stringByDeletingPathExtension] ofType:[imageName pathExtension]];
	if(path){
		CGImageSourceRef imageSource = CGImageSourceCreateWithURL((__bridge CFURLRef)[NSURL fileURLWithPath:path], NULL);
		
		if(imageSource){
			returnValue = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
		}
	}
	
	return returnValue;
}

