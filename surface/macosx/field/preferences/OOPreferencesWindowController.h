//
//  PreferencesAppDelegate.h
//  Surface
//
//  Created by Charles Guillot on 03/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface OOPreferencesWindowController : NSWindowController <NSToolbarDelegate> 
{
    IBOutlet NSToolbar *bar;
	IBOutlet NSView *generalPreferenceView;
	IBOutlet NSView *accountPreferenceView;
	IBOutlet NSView *networkPreferenceView;
	IBOutlet NSView *advancedPreferenceView;
    
    IBOutlet NSTextField *accountAccountName;
    IBOutlet NSTextField *accountComputerName;
	int currentViewTag;
}
+ (OOPreferencesWindowController *)getInstance;
- (NSView *)viewForTag:(int)tag;
- (IBAction)switchView:(id)sender;
- (NSRect)newFrameForNewContentView:(NSView *)view;

@end
