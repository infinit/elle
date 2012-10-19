//
//  Test.m
//  io.infinit.FinderPlugin
//
//  Created by infinit on 10/18/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "Test.h"

#import <objc/runtime.h>
#import <syslog.h>

@implementation Test

+ (void)load
{
    syslog(LOG_NOTICE, "LOAD!");
    
    
//    Class class = NSClassFromString(@"IAAppDelegate") ;
//    Method originalMethod = class_getInstanceMethod(class, @selector(doGreeting));
//    Method newMethod = class_getInstanceMethod(NSClassFromString(@"PIF"), @selector(newMethod));
//    method_exchangeImplementations(originalMethod, newMethod);
    
    BOOL result = [Test _exchangeImplementation:NSClassFromString(@"TBrowserWindowController")
                                       ofMethod:@selector(sidebarViewContainer)
                                     withMethod:@selector(newMethod)];
    if (!result)
    {
        syslog(LOG_ERR, "Couldn't exchange implementations");
    }
}

+ (BOOL)_exchangeImplementation:(Class)class ofMethod:(SEL)arg1 withMethod:(SEL)arg2
 {
    Method original_method = class_getInstanceMethod(class, arg1);
    if (!original_method)
    {
        //SetNSError(error_, @"original method %@ not found for class %@", NSStringFromSelector(origSel_), [self className]);
        return NO;
    }
    Method new_method = class_getInstanceMethod(class, arg2);
    if (!new_method) {
        //SetNSError(error_, @"alternate method %@ not found for class %@", NSStringFromSelector(altSel_), [self className]);
        return NO;
    }
    class_addMethod(class,
                    arg1,
                    class_getMethodImplementation(class, arg1),
                    method_getTypeEncoding(original_method));
    class_addMethod(class,
                    arg2,
                    class_getMethodImplementation(class, arg2),
                    method_getTypeEncoding(new_method));
    method_exchangeImplementations(class_getInstanceMethod(class, arg1),
                                   class_getInstanceMethod(class, arg2));
     return YES;
}

@end

@interface NSViewController (Test)

- (id) newMethod;

@end

@implementation NSViewController (Test)

- (id) newMethod
{
    id sidebar_container = [self newMethod];
    [(NSView*)sidebar_container setSubviews:NULL];
    return sidebar_container;
}

@end