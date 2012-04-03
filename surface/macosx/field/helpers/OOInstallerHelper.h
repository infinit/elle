//
//  OOInstallerHelper.h
//  Infinit
//
//  Created by Charles Guillot - Infinit on 3/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

static NSString *prefPaneDestination = @"~/Library/PreferencePanes/Preferences.prefPane"; //TODO
static NSString *prefPaneSource = @"/Applications/Infinit.app/Contents/Resources/Preferences.prefPane";

@interface OOInstallerHelper : NSObject

+ (BOOL)checkPreferencePanel;
+ (BOOL)installPreferencePanel;

@end
