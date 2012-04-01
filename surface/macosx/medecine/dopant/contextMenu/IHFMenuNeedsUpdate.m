//
//  IHFDrawIconWithFrame.m
//  InfinitHelperTools
//
//  Created by Charles Guillot - Infinit on 3/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "IHFMenuNeedsUpdate.h"

@implementation IHFMenuNeedsUpdate

+ (void) initialize
{
    [NSClassFromString(@"TColumnViewController") 
     fi_swizzleMethod:@selector(menuNeedsUpdate:) 
     withMethod:@selector(new_menuNeedsUpdate:) 
     error:NULL];
}

- (id) init
{
    self = [super init];
    
    if(self) {
        
        // Init
	}
    return self;
}
+ (IHFMenuNeedsUpdate*) instance
{
	static IHFMenuNeedsUpdate *item = nil;
	
	if(item == nil)
		item = [[IHFMenuNeedsUpdate alloc] init];
	
	return item;
}
/*
- (id) init
{
    sel_OldMethod = @selector(menuNeedsUpdate:);
    sel_NewMethod = @selector(new_menuNeedsUpdate:);
    classType = @"v@";
    return [super init];
}
- (void) loadTListViewController
{
    switchedClass = @"TDesktopViewController";
    [super load];
    class_addMethod(targetClass, @selector(openEchowavesURL:), class_getMethodImplementation(selfClass, @selector(openEchowavesURL:)),"v@");
}
- (void) loadTColumnViewController
{
    
    switchedClass = @"TIconViewController";
    [super load];
    class_addMethod(targetClass, @selector(openEchowavesURL:), class_getMethodImplementation(selfClass, @selector(openEchowavesURL:)),"v@");
}
- (void) loadTIconViewController
{
    sel_OldMethod = @selector(menuForEvent:);
    sel_NewMethod = @selector(new_menuForEvent:);
    classType = @"@@";
    switchedClass = @"TIconViewController";
    [super load];
}
- (id)new_menuForEvent:(id)arg1
{
    //[self new_menuForEvent:arg1 ];
    id theMenu =  [self new_menuForEvent:arg1 ];
    [(NSMenu *)theMenu insertItem:[NSMenuItem separatorItem] atIndex:2];
    NSMenuItem *openItem = [(NSMenu *)theMenu insertItemWithTitle:@"Hello Infinit" action:@selector(openEchowavesURL:) keyEquivalent:@"" atIndex:3];
    [(NSMenu *)theMenu insertItem:[NSMenuItem separatorItem] atIndex:4];
    [openItem setTarget:self];
    return Nil;
    
}
- (void) load
{
    [self loadTListViewController];
    [self loadTColumnViewController];
    //[self loadTIconViewController];
}*/
@end

@implementation NSViewController (IHFMenuNeedsUpdate)

- (void) new_menuNeedsUpdate:(id)arg1
{
    [self new_menuNeedsUpdate:arg1 ];
    [arg1 insertItem:[NSMenuItem separatorItem] atIndex:2];
    NSMenuItem *infItem = [arg1 insertItemWithTitle:@"Infinit" action:nil keyEquivalent:@"" atIndex:3];
    NSMenu *submenu = [[NSMenu alloc] init];
    NSMenuItem *infMenuItem = [submenu addItemWithTitle:@"Share this folder ..." action:@selector(openEchowavesURL:) keyEquivalent:@""];
    [infItem setSubmenu:submenu];
    [arg1 insertItem:[NSMenuItem separatorItem] atIndex:4];
    [infMenuItem setTarget:self];
}

- (void) openEchowavesURL:(id)sender {
    // function details here
    NSLog(@"Hello Infinit !!!!!");
}

@end
