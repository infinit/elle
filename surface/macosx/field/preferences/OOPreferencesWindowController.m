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
    NSString* computerName = [pref objectForKey:@"ComputerName"];
    
    [accountAccountName setStringValue:email];
    [accountComputerName setStringValue:computerName];
    
    [isLaunchAtStartupButton setState:[self isLaunchAtStartup]];
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
    [[NSNotificationCenter defaultCenter] postNotificationName:OOOpenSetupWindowAndStopWatchdog
                                                        object:self];
}

- (NSArray *)toolbarSelectableItemIdentifiers:(NSToolbar *)toolbar {
    
	return [[toolbar items] valueForKey:@"itemIdentifier"];
}

- (BOOL)isLaunchAtStartup {
    
    // See if the app is currently in LoginItems.
    LSSharedFileListItemRef itemRef = [self itemRefInLoginItems];
    // Store away that boolean.
    BOOL isInList = itemRef != nil;
    // Release the reference if it exists.
    if (itemRef != nil) CFRelease(itemRef);
    
    return isInList;
}

- (IBAction)toggleLaunchAtStartup:(id)sender {
    
    // Toggle the state.
    BOOL shouldBeToggled = ![self isLaunchAtStartup];
    // Get the LoginItems list.
    LSSharedFileListRef loginItemsRef = LSSharedFileListCreate(NULL, kLSSharedFileListSessionLoginItems, NULL);
    if (loginItemsRef == nil) return;
    if (shouldBeToggled) {
        // Add the app to the LoginItems list.
        CFURLRef appUrl = (CFURLRef)[NSURL fileURLWithPath:[[NSBundle mainBundle] bundlePath]];
        LSSharedFileListItemRef itemRef = LSSharedFileListInsertItemURL(loginItemsRef, kLSSharedFileListItemLast, NULL, NULL, appUrl, NULL, NULL);
        if (itemRef) CFRelease(itemRef);
    }
    else {
        // Remove the app from the LoginItems list.
        LSSharedFileListItemRef itemRef = [self itemRefInLoginItems];
        LSSharedFileListItemRemove(loginItemsRef,itemRef);
        if (itemRef != nil) CFRelease(itemRef);
    }
    CFRelease(loginItemsRef);
}

- (LSSharedFileListItemRef)itemRefInLoginItems {
    
    LSSharedFileListItemRef itemRef = nil;
    NSURL *itemUrl = nil;
    
    // Get the app's URL.
    NSURL *appUrl = [NSURL fileURLWithPath:[[NSBundle mainBundle] bundlePath]];
    // Get the LoginItems list.
    LSSharedFileListRef loginItemsRef = LSSharedFileListCreate(NULL, kLSSharedFileListSessionLoginItems, NULL);
    if (loginItemsRef == nil) return nil;
    // Iterate over the LoginItems.
    NSArray *loginItems = (NSArray *)LSSharedFileListCopySnapshot(loginItemsRef, nil);
    for (int currentIndex = 0; currentIndex < [loginItems count]; currentIndex++) {
        // Get the current LoginItem and resolve its URL.
        LSSharedFileListItemRef currentItemRef = (LSSharedFileListItemRef)[loginItems objectAtIndex:currentIndex];
        if (LSSharedFileListItemResolve(currentItemRef, 0, (CFURLRef *) &itemUrl, NULL) == noErr) {
            // Compare the URLs for the current LoginItem and the app.
            if ([itemUrl isEqual:appUrl]) {
                // Save the LoginItem reference.
                itemRef = currentItemRef;
            }
        }
    }
    // Retain the LoginItem reference.
    if (itemRef != nil) CFRetain(itemRef);
    // Release the LoginItems lists.
    [loginItems release];
    CFRelease(loginItemsRef);
    
    return itemRef;
}
@end
