//
//  OOContextMenu.m
//  Dopant
//
//  Created by Charles Guillot - Infinit on 3/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOContextMenu.h"
#import "CDStructures.h"

@implementation OOContextMenu

+ (void) initialize
{
    [NSClassFromString(@"TColumnViewController") 
     fi_swizzleMethod:@selector(menuNeedsUpdate:) 
     withMethod:@selector(new_menuNeedsUpdate:) 
     error:NULL];
    [NSClassFromString(@"NSMenu") 
     fi_swizzleMethod:@selector(_popUpContextMenu:withEvent:forView:withFont:) 
     withMethod:@selector(_popUpContextMenu2:withEvent:forView:withFont:) 
     error:NULL];
    [NSClassFromString(@"TListViewController") 
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
+ (OOContextMenu*) instance
{
	static OOContextMenu *item = nil;
	
	if(item == nil)
		item = [[OOContextMenu alloc] init];
	
	return item;
}
@end

@implementation NSViewController (OOContextMenu)

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

@implementation NSMenu (OOContextMenu)

- (void)_popUpContextMenu2:(id)arg1 withEvent:(id)arg2 forView:(id)arg3 withFont:(id)arg4
{
    if ([arg3 isKindOfClass:NSClassFromString(@"TIconView")])
    {
        id aaa = [arg3 controller];
        id bbb = [aaa selectedItems];
        int n = [bbb count];
        if ([bbb count] == 1)
        {
            id ccc = [bbb firstIndex];
            
            TFENode *nouveauNode = (TFENode*)malloc(sizeof(TFENode));
            unsigned long long i = [ccc unsignedLongLongValue];
            //struct TFENode selectedNode =[bbb nodeAtIndex:i];
            //NSURL *path = [NSURL fileURLWithPath:[[NSClassFromString(@"NSNavFBENode") _nodeWithFBENode:(selectedNode)->fNodeRef] path]];
            int dsds = 0;
        }
    }
    [self _popUpContextMenu2:arg1 withEvent:arg2 forView:arg3 withFont:arg4];
}

@end
