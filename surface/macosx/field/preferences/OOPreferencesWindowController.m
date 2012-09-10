//
//  PreferencesAppDelegate.m
//  Surface
//
//  Created by Charles Guillot on 03/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOPreferencesWindowController.h"
#import "AppDelegate.h"

@implementation OOPreferencesWindowController

+ (OOPreferencesWindowController *)getInstance {
    
    static OOPreferencesWindowController *singleton;
    @synchronized(self){
        if (![singleton window]){
            // Le singleton n'a pas encore été instancié
            singleton = [[OOPreferencesWindowController alloc] init];
        }
        return singleton;
    }
    
}

- (id) init {
    if(self = [super initWithWindowNibName:@"OOPreferencesWindows"]) {
    }
    return self;
}

-(void)awakeFromNib{
	[self.window setContentSize:[generalPreferenceView frame].size];
	[[self.window contentView] addSubview:generalPreferenceView];
	[bar setSelectedItemIdentifier:@"General"];
    [self.window setLevel:NSScreenSaverWindowLevel + 1];
    
    NSUserDefaults *pref;
    pref=[NSUserDefaults standardUserDefaults];
    NSString* email = [pref objectForKey:@"Email"];
    NSString* password = [pref objectForKey:@"Password"];
    NSString* computerName = [pref objectForKey:@"ComputerName"];
    
    [accountAccountName setStringValue:email];
    [accountComputerName setStringValue:computerName];
}

- (NSView *)viewForTag:(int)tag {
    NSView *view = nil;
	switch(tag) {
		case 0: default: view = generalPreferenceView; break;
		case 1: view = accountPreferenceView; break;
		case 2: view = networkPreferenceView; break;
		case 3: view = advancedPreferenceView; break;
	}
    return view;
}

- (IBAction)switchView:(id)sender {
	
	int tag = (int)[sender tag];
	
	NSView *view = [self viewForTag:tag];
	NSView *previousView = [self viewForTag: currentViewTag];
	currentViewTag = tag;
	NSRect newFrame = [self newFrameForNewContentView:view];
    
	[NSAnimationContext beginGrouping];
	[[NSAnimationContext currentContext] setDuration:0.1];
	
	if ([[NSApp currentEvent] modifierFlags] & NSShiftKeyMask)
	    [[NSAnimationContext currentContext] setDuration:1.0];
	
	[[[self.window contentView] animator] replaceSubview:previousView with:view];
	[[self.window animator] setFrame:newFrame display:YES];
	
	[NSAnimationContext endGrouping];
	
}

- (NSRect)newFrameForNewContentView:(NSView *)view {
	
    NSRect newFrameRect = [self.window frameRectForContentRect:[view frame]];
    NSRect oldFrameRect = [self.window frame];
    NSSize newSize = newFrameRect.size;
    NSSize oldSize = oldFrameRect.size;    
    NSRect frame = self.window.frame;
    frame.size = newSize;
    frame.origin.y -= (newSize.height - oldSize.height);
    
    newFrameRect = view.frame;
    newFrameRect.origin.y -= (newSize.height - oldSize.height);
    [view setFrame:newFrameRect];
    
    return frame;
}

- (IBAction)unlinkInfinitAccount:(id)sender {
    NSUserDefaults *pref;
    pref=[NSUserDefaults standardUserDefaults];
    [pref setObject:@"NO" forKey:@"RememberMe"];
    [pref synchronize];
    [[self window] close];
    [[NSNotificationCenter defaultCenter] postNotificationName:OOOpenSetupWindowAndStopWatchdog object:self];
}

- (NSArray *)toolbarSelectableItemIdentifiers:(NSToolbar *)toolbar {
	return [[toolbar items] valueForKey:@"itemIdentifier"];
}

@end
