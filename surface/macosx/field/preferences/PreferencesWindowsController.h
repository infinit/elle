//
//  PreferencesAppDelegate.h
//  Surface
//
//  Created by Charles Guillot on 03/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface PreferencesWindowsController : NSWindowController <NSToolbarDelegate> 
{
    IBOutlet NSToolbar *bar;
	IBOutlet NSView *generalPreferenceView;
	IBOutlet NSView *accountPreferenceView;
	IBOutlet NSView *networkPreferenceView;
	IBOutlet NSView *advancedPreferenceView;
	
	int currentViewTag;
}

- (NSView *)viewForTag:(int)tag;
- (IBAction)switchView:(id)sender;
- (NSRect)newFrameForNewContentView:(NSView *)view;

@end
