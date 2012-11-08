//
//  IAStatusItem.m
//  FinderWindowApplication
//
//  Created by infinit on 11/7/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAStatusItem.h"

@implementation IAStatusItem
{
    NSStatusItem*    _status_item;
    NSImage*         _default_status_icon;
}

+ (void) load
{
    //@autoreleasepool {
        [self _instance];
    //}
}

+ (IAStatusItem*)_instance
{
    __strong static id instance = nil;
    
    @synchronized(self) {
    if (instance == nil)
        instance = [[self alloc] _init];
    }
    return instance;
}

- (id)_init
{
    self = [super init];
    if (self)
    {
        int i = 0;
        NSLog(@"Pif %d", i++);
        NSStatusBar* main_status_bar = [NSStatusBar systemStatusBar];
                NSLog(@"Pif %d", i++);
        _status_item = [main_status_bar statusItemWithLength:NSVariableStatusItemLength];
                NSLog(@"Pif %d", i++);
        if (_default_status_icon == nil)
        {
                    NSLog(@"Pif %d", i++);
            NSBundle* bundle = [NSBundle bundleWithIdentifier:@"io.infinit.FinderWindow"];
                    NSLog(@"Pif %d", i++);
            NSString* path = [bundle pathForResource:@"active_status_icon" ofType:@"png"];
                NSLog(@"Pif %d", i++);
            _default_status_icon = [[NSImage alloc] initWithContentsOfFile:path];
                    NSLog(@"Pif %d", i++);
            [_default_status_icon setTemplate:YES];
                    NSLog(@"Pif %d", i++);
        }
        
        [_status_item setImage:_default_status_icon];
                NSLog(@"Pif %d", i++);
        [_status_item setHighlightMode:YES];
                NSLog(@"Pif %d", i++);
    }
    return self;
}

-(void) dealloc
{
    NSLog(@"DEALLOC");
}

@end
