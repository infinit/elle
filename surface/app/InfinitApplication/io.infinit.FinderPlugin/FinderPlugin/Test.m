//
//  Test.m
//  io.infinit.FinderPlugin
//
//  Created by infinit on 10/18/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "Test.h"
#import "IPCInterface.h"
#import "config.h"

#import <objc/runtime.h>
#import <syslog.h>

#import <FinderWindow/IAFinderWindowController.h>
#import <FinderWindow/IAFinderWindow.h>


#define CSTR(o) ([o isKindOfClass:[NSArray class]]          \
? [[o componentsJoinedByString: @", "] UTF8String]      \
: [[NSString stringWithFormat:@"%@", o] UTF8String])    \
/**/

#define CLS(cls) NSClassFromString(@#cls)


# define SETUP_HOOK_DYNAMIC_METHOD(cls, hook)  do {  \
    BOOL result = [Test _exchangeImplementation:NSClassFromString(@#cls)        \
ofMethod:@selector(hook)                                     \
withMethod:@selector(_hook_ ## hook)];                         \
if (!result)                                                                                    \
{                                                                                               \
syslog(LOG_ERR, "Couldn't exchange implementations of " #hook);                             \
return;                                                                                     \
}                                                                                               \
else                                                                                            \
syslog(LOG_NOTICE, "Successfully hooked " #hook " method");                                                  \
} while(false)
/**/

# define HOOK_NAV_SETUP(hook)      do {                                                                     \
BOOL result = [Test _exchangeStaticImplementation:NSClassFromString(@"NSNavFBENode")        \
ofMethod:@selector(hook)                                     \
withMethod:@selector(_hook_ ## hook)];                         \
if (!result)                                                                                    \
{                                                                                               \
syslog(LOG_ERR, "Couldn't exchange implementations of " #hook);                             \
return;                                                                                     \
}                                                                                               \
else                                                                                            \
syslog(LOG_NOTICE, "Successfully hooked " #hook " method");                                                  \
} while(false)
/**/


@interface NSWindowController (Test)

- (BOOL)_isInfinitWindow;
- (void)_initializeInfinitWindow;

@end

@interface WindowLoader : NSObject

+ (WindowLoader*) instance;

- (void) initialize;
@property (retain) IAFinderWindowController* window_controller;

@end

@implementation WindowLoader

+ (WindowLoader*) instance
{
    static WindowLoader* i = nil;
    if (i == nil)
    {
        i = [[WindowLoader alloc] init];
        [i retain];
    }
    return i;
}

- (void) initialize
{
    id windows = [NSApp windows];
    for (NSWindow* window in windows)
    {
        NSWindowController* window_controller = [window windowController];
        if (window_controller != nil && [window_controller _isInfinitWindow])
            [window_controller _initializeInfinitWindow];
    }
}

+ (IAFinderWindow*) retreiveInfinitWindowFor:(NSWindow*)browserWindow
{
    for (NSWindow* w in [browserWindow childWindows])
    {
        if ([w isKindOfClass:[IAFinderWindow class]])
        {
            return (IAFinderWindow *)w;
        }
    }
    return nil;
}

+ (IAFinderWindow*) retreiveOrCreateInfinitWindowFor:(NSWindow*)browserWindow
{
    IAFinderWindow* window = [WindowLoader retreiveInfinitWindowFor:browserWindow];
    if (window == nil)
    {
        IAFinderWindowController* window_controller = [[IAFinderWindowController alloc] init];
        window = (IAFinderWindow*)[window_controller window];
        [browserWindow addChildWindow:window
                              ordered:NSWindowAbove];
    }
    assert(window != nil);
    return window;
}

@end

@implementation Test


+ (void)load
{
    syslog(LOG_NOTICE, "Loading injector");
    
    
    SETUP_HOOK_DYNAMIC_METHOD(TSidebarViewController, zoneList:);
    //    SETUP_HOOK_DYNAMIC_METHOD(TSidebarViewController, zoneForCell:);
    //    SETUP_HOOK_DYNAMIC_METHOD(TSidebarViewController, zoneHeaderForKind:);
    //    SETUP_HOOK_DYNAMIC_METHOD(TSidebarViewController, zoneHeaderForZone:);
    SETUP_HOOK_DYNAMIC_METHOD(TSidebarItemCell, drawWithFrame:inView:);
    SETUP_HOOK_DYNAMIC_METHOD(TBrowserWindowController, commonFinishInitialization);
    
    SETUP_HOOK_DYNAMIC_METHOD(TColumnViewController, menuNeedsUpdate:);
    SETUP_HOOK_DYNAMIC_METHOD(TListViewController, menuNeedsUpdate:);
    SETUP_HOOK_DYNAMIC_METHOD(NSMenu, _popUpContextMenu:withEvent:forView:withFont:);
    SETUP_HOOK_DYNAMIC_METHOD(TIconViewController, nodeAtIndex:);
    
    HOOK_NAV_SETUP(specialNodeOfType:);
    

    [[WindowLoader instance] initialize];
}



+ (BOOL)_exchangeImplementation:(Class)cls ofMethod:(SEL)arg1 withMethod:(SEL)arg2
 {
    Method original_method = class_getInstanceMethod(cls, arg1);
    if (!original_method)
    {
        syslog(LOG_ERR, "Cannot find original method");
        //SetNSError(error_, @"original method %@ not found for class %@", NSStringFromSelector(origSel_), [self className]);
        return NO;
    }
    Method new_method = class_getInstanceMethod(cls, arg2);
    if (!new_method) {
        syslog(LOG_ERR, "Cannot find new method");
        //SetNSError(error_, @"alternate method %@ not found for class %@", NSStringFromSelector(altSel_), [self className]);
        return NO;
    }
    class_addMethod(cls,
                    arg1,
                    class_getMethodImplementation(cls, arg1),
                    method_getTypeEncoding(original_method));
    class_addMethod(cls,
                    arg2,
                    class_getMethodImplementation(cls, arg2),
                    method_getTypeEncoding(new_method));
    method_exchangeImplementations(class_getInstanceMethod(cls, arg1),
                                   class_getInstanceMethod(cls, arg2));
     return YES;
}

+ (BOOL)_exchangeStaticImplementation:(Class)cls ofMethod:(SEL)arg1 withMethod:(SEL)arg2
{
    Method original_method = class_getClassMethod(cls, arg1);
    if (!original_method)
    {
        syslog(LOG_ERR, "Cannot find original method");
        //SetNSError(error_, @"original method %@ not found for class %@", NSStringFromSelector(origSel_), [self className]);
        return NO;
    }
    Method new_method = class_getClassMethod(cls, arg2);
    if (!new_method) {
        syslog(LOG_ERR, "Cannot find new method");
        //SetNSError(error_, @"alternate method %@ not found for class %@", NSStringFromSelector(altSel_), [self className]);
        return NO;
    }
    class_addMethod(cls,
                    arg1,
                    class_getMethodImplementation(cls, arg1),
                    method_getTypeEncoding(original_method));
    class_addMethod(cls,
                    arg2,
                    class_getMethodImplementation(cls, arg2),
                    method_getTypeEncoding(new_method));
    method_exchangeImplementations(class_getClassMethod(cls, arg1),
                                   class_getClassMethod(cls, arg2));
    return YES;
}

+ (NSString*)getDrivePath
{
    static NSString* path = nil;
    
    if (path == nil)
    {
        
        IPCInterface* ipc_proxy = (IPCInterface*)[NSConnection rootProxyForConnectionWithRegisteredName:INFINIT_DAEMON_LABEL host:nil];
        
        if (ipc_proxy == nil)
        {
            syslog(LOG_ERR, "Couldn't connect to the infinit daemon");
            return nil;
        }
        path = [ipc_proxy.drive_path retain];
        syslog(LOG_NOTICE, "Got drive path %s", CSTR(path));
    }
    return path;
}

+ (id)getSidebarImage
{
    static id img = nil;
    if (img == nil)
    {
        NSString* path = [[NSBundle bundleWithIdentifier:@"io.infinit.FinderPlugin"] pathForResource:@"18px-sidebar-active" ofType:@"png"];
        img = [[NSImage alloc] initWithContentsOfFile:path];
        [img setTemplate:YES];
    }
    return img;
}

@end


@implementation NSObject (Test)

+ (id)_hook_specialNodeOfType:(unsigned int)arg1
{
    id res = [NSObject _hook_specialNodeOfType:arg1];
    syslog(LOG_NOTICE, "specialOfType:%d -> %s", arg1, CSTR(res));
    return res;
}

@end

@interface TSidebarItemCell

@end

struct TFENode
{
    void* opaque_node_ref;
    void* tab[4];
};


@implementation NSViewController (Test)

- (struct TFENode)_hook_nodeAtIndex:(unsigned long long)arg1
{
    // Helper to cast to the right structure
    return [self _hook_nodeAtIndex:arg1];
}
- (void) _hook_menuNeedsUpdate:(id)arg1
{
    [self _hook_menuNeedsUpdate:arg1 ];
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

- (id)_hook_zoneList:(int)arg1
{
    id initial_list = [self _hook_zoneList:arg1];
    if (arg1 != 3 /*FAVORITES*/)
        return initial_list;
    
    NSString* path = [Test getDrivePath];
    if (path == nil)
    {
        syslog(LOG_ERR, "Cannot retreive virtual drive path !");
        return initial_list;
    }
    BOOL found = false;
    for (id zone in initial_list)
    {
        if (zone == nil)
            continue;
        NSString* node_path = [[zone accessibilityAttributeValue:@"AXURL"] path];
        //syslog(LOG_NOTICE, "FOUND ZONE: %s with path = %s", CSTR(zone), CSTR(node_path));
        if ([path isEqualToString:node_path])
        {
            syslog(LOG_NOTICE, "FOUND Infinit: %s with path = %s", CSTR(zone), CSTR(node_path));
            found = true;
        }
        
    }
    
    if (found || initial_list == nil)
        return initial_list;
    syslog(LOG_NOTICE, "CREATING NEW NODE %s", CSTR(initial_list));
    TFENode* tfe_node = (TFENode*)calloc(1, sizeof(TFENode));
    id nav_node = [CLS(NSNavFBENode) nodeWithPath:path];
    tfe_node->opaque_node_ref = [nav_node fbeNode];
    //id item_cell = [[CLS(TSidebarItemCell) alloc] initWithNode:tfe_node orZone:1];
    [nav_node addToFavoritesAtIndex:1];
            syslog(LOG_NOTICE, "New fbe node %s: path = %s, URL = %s, parent = %s, parent.URL = %s, parentVolume = %s",
                   CSTR(nav_node),
                   CSTR([nav_node performSelector:@selector(path)]),
                   CSTR([nav_node performSelector:@selector(URL)]),
                   CSTR([nav_node performSelector:@selector(parent)]),
                   CSTR([[nav_node performSelector:@selector(parent)] performSelector:@selector(URL)]),
                   CSTR([nav_node performSelector:@selector(parentVolume)]));
        
//        BOOL found = false;
//        for (id cell in res)
//        {
//            if ([cell name] == @"Infinit")
//            {
//                found = true;
//                break;
//            }
//        }
//        if (!found)
//        {
////            syslog(LOG_NOTICE, "NOT FOUND");
////            
////            id new_node = [[CLS(NSNavVirtualNode) alloc] init];
////            [new_node performSelector:@selector(setName:) withObject:@"NAME DE LA NODE"];
////            syslog(LOG_NOTICE, "New node: %s, name = %s, path = %s", CSTR(new_node), [[new_node name] UTF8String], [[new_node path] UTF8String]);
////            
////            id new_fbe_node = [[CLS(NSNavFBENode) alloc] initWithPath:@""];
//            //[new_node performSelector:@selector(setOriginalNode:) withObject:new_cell];
///*            SEL sel = @selector(initTextCell:);
//            id new_cell = [NSClassFromString(@"TSidebarItemCell") alloc];
//            NSInvocation* inv = [NSInvocation invocationWithMethodSignature:[new_cell methodSignatureForSelector:sel]];
//            [inv setSelector:sel];
//            [inv setTarget:new_cell];
//            NSString* text = @"Cell text";
//            [inv setArgument:&text atIndex:2];
////            void* ptr= NULL;
////            [inv setArgument:&ptr atIndex:2];
////            int i = 3;
////            [inv setArgument:&i atIndex:3];
//            [inv invoke];
//            [new_cell performSelector:@selector(setTitle:) withObject:@"CELL TITLE"];
//            [new_cell performSelector:@selector(setStringValue:) withObject:@"CELL STRING VALUE"];*/
//            //syslog(LOG_NOTICE, "New cell: %s", CSTR(new_cell));
//        }

    
    return [self _hook_zoneList:arg1];
}

- (int)_hook_zoneForCell:(id)arg1
{
    int res = [self _hook_zoneForCell:arg1];
    //syslog(LOG_NOTICE, "_hook_zoneForCell(%s) -> %d", CSTR(arg1), res);
    return res;
}

- (id)_hook_zoneHeaderForKind:(int)arg1
{
    id res = [self _hook_zoneHeaderForKind:arg1];
    //syslog(LOG_NOTICE, "_hook_zoneHeaderForKind(%d) -> %s", arg1, CSTR(res));
    return res;
}

- (id)_hook_zoneHeaderForZone:(id)arg1
{
    id res = [self _hook_zoneHeaderForZone:arg1];
//    for (id o in arg1)
//    {
//        
//        NSString* r = (NSString*)[o performSelector:@selector(name)];
//        syslog(LOG_NOTICE, "item cell %s: node == %s", CSTR(o), );
//    }
    //syslog(LOG_NOTICE, "_hook_zoneHeaderForZone(%s) -> %s", CSTR(arg1), CSTR(res));
    return res;
}

@end



@implementation NSTextFieldCell (Test)

- (void)_hook_drawWithFrame:(CGRect)arg1 inView:(id)arg2
{
    NSString* drive_path = [Test getDrivePath];
    if ([self respondsToSelector:@selector(accessibilityAttributeValue:)])
    {
        NSString* node_path = [[self performSelector:@selector(accessibilityAttributeValue:) withObject:@"AXURL"] path];
        if ([node_path isEqualToString:drive_path])
        {
            [self setStringValue:@"Infinit2"];
            if ([self respondsToSelector:@selector(image)])
            {
                id img = [self image];
                if ([img respondsToSelector:@selector(initWithSourceImage:)])
                {
                    [img initWithSourceImage:[Test getSidebarImage]];
                }
                else if ([self respondsToSelector:@selector(setImage:)])
                {
                    [self setImage:[Test getSidebarImage]];
                }
            }
        }
    }
    else
        syslog(LOG_WARNING, "A text field cell does not has accessibility ...");
    [self _hook_drawWithFrame:arg1 inView:arg2];
}

@end


@implementation NSWindowController (Test)

- (void)_hook_commonFinishInitialization
{
    [self _hook_commonFinishInitialization];
    [self _initializeInfinitWindow];
    if ([self _isInfinitWindow])
    {
        [[[WindowLoader retreiveInfinitWindowFor:[self window]] windowController] showWindow:self];
        [[WindowLoader retreiveInfinitWindowFor:[self window]] orderFront:self];
    }
    else
        [[WindowLoader retreiveInfinitWindowFor:[self window]] orderOut:self];
}

- (BOOL)_isInfinitWindow
{
    id view_controller = [self performSelector:@selector(browserViewController)];
    if (view_controller != nil)
    {
        TFENode* target = (TFENode*)[view_controller performSelector:@selector(target)];
        if (target != nil)
        {
            NSString* path = [[CLS(NSNavFBENode) _nodeWithFBENode:target->opaque_node_ref] performSelector:@selector(path)];
            if ([path isEqualToString:[Test getDrivePath]])
                return YES;
        }
        else
            syslog(LOG_ERR, "Couldn't retreive the view controller target");
    }
    else
        syslog(LOG_ERR, "Couldn't retreive the view controller.");
    return NO;
}

- (void)_initializeInfinitWindow
{
    //[[self window] setTitle:@"MEGA TITLE DE OUF"];
    NSWindow* window = [WindowLoader retreiveOrCreateInfinitWindowFor:[self window]];
    
    [window setFrame:[self getInfinitWindowsFrameFor:window]
             display:YES
             animate:NO];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(receiveUpdateFrameNotification:)
                                                 name:NSWindowDidResizeNotification
                                               object:nil ];
    //[[window windowController] showWindow:self];
}


- (void)receiveUpdateFrameNotification:(NSNotification *) notification {
    IAFinderWindow* window = [WindowLoader retreiveOrCreateInfinitWindowFor:[self window]];
    if (window != nil) {
        [window setFrame:[self getInfinitWindowsFrameFor:window]
                 display:YES];
    }
}
//
//- (NSString *)windowTitleForDocumentDisplayName:(NSString *)displayName
//{
//    return @"BEGA OUFLE DE ITLE";
//}


- (NSRect)getInfinitWindowsFrameFor:(IAFinderWindow*)arg1 {
    id browserWindow = [self browserWindow];
    id browserViewContainer = [self browserViewContainer];
    id browserViewController = [self browserViewController];
    id browserView = [browserViewController view];
    NSRect windowFrame = [browserWindow frame];
    NSRect browserViewContainerFrame = [browserViewContainer frame];
    NSRect browserViewFrame = [browserView frame];
    
    CGFloat x = windowFrame.origin.x + browserViewFrame.origin.x + browserViewContainerFrame.origin.x;
    CGFloat y = windowFrame.origin.y + browserViewFrame.origin.y + browserViewContainerFrame.origin.y;
    CGFloat w = browserViewFrame.size.width;
    CGFloat h = browserViewFrame.size.height - 1;
    NSRect frame = [arg1 frame];
    if (arg1 == Nil) {
        frame = NSMakeRect(x, y, w, h);
    } else {
        frame = [arg1 frame];
        frame.origin.x = x;
        frame.origin.y = y;
        frame.size.width = w;
        frame.size.height = h;
    }
    return frame;
}


@end


@implementation NSMenu (Test)

- (void)_hook__popUpContextMenu:(id)arg1 withEvent:(id)arg2 forView:(id)arg3 withFont:(id)arg4
{
    if ([arg3 isKindOfClass:NSClassFromString(@"TIconView")])
    {
        id controller = [arg3 controller];
        id bbb = [controller selectedItems];
        int n = [bbb count];
        if ([bbb count] == 1)
        {
            unsigned long long int selected_menu_node_index = [bbb firstIndex];
            TFENode selected_menu_node = [controller _hook_nodeAtIndex:selected_menu_node_index];
            NSLog([[NSClassFromString(@"NSNavFBENode") _nodeWithFBENode:selected_menu_node.opaque_node_ref] path]);
        }
        [self insertItem:[NSMenuItem separatorItem] atIndex:2];
        NSMenuItem *infItem = [self insertItemWithTitle:@"Infinit" action:nil keyEquivalent:@"" atIndex:3];
        NSMenu *submenu = [[NSMenu alloc] init];
        NSMenuItem *infMenuItem = [submenu addItemWithTitle:@"Share this folder ..." action:@selector(openEchowavesURL:) keyEquivalent:@""];
        [infItem setSubmenu:submenu];
        [self insertItem:[NSMenuItem separatorItem] atIndex:4];
        [infMenuItem setTarget:controller];
    }
    [self _hook__popUpContextMenu:arg1 withEvent:arg2 forView:arg3 withFont:arg4];
}

@end
