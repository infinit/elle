//
//  OOCellDictKey.m
//  Dopant
//
//  Created by Charles Guillot on 28/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOCellDictKey.h"
#import <objc/objc-class.h>
#import "OOZombieDictKey.h"

@implementation OOCellDictKey

@synthesize nodeStatus;

- (unsigned long long)hash
{
    [NSException raise:NSInternalInconsistencyException 
                       format:@"You must override %@ in a subclass", NSStringFromSelector(_cmd)];
    return 0;
}

- (BOOL)isEqual:(id)arg1
{
    [NSException raise:NSInternalInconsistencyException 
                format:@"You must override %@ in a subclass", NSStringFromSelector(_cmd)];
    return NO;
}

- (NSURL *) getPath
{
    [NSException raise:NSInternalInconsistencyException 
                format:@"You must override %@ in a subclass", NSStringFromSelector(_cmd)];
    return nil;
}

- (void) refreshCell
{
    [NSException raise:NSInternalInconsistencyException 
                format:@"You must override %@ in a subclass", NSStringFromSelector(_cmd)];
}

- (void) cleanUp
{
    [NSException raise:NSInternalInconsistencyException 
                format:@"You must override %@ in a subclass", NSStringFromSelector(_cmd)];
    
}

- (void) main
{
    NSAutoreleasePool *oppPool = [[NSAutoreleasePool alloc] init];
    if (self != nil && !self.isCancelled)
    {
        NSURL* url = [self getPath];
        NSString *filename = [[url path] lastPathComponent];
        
        NSString *firstChar = [filename substringToIndex:1];
        
        // Get status by path
        sleep(1);
        // Set status
        if ([firstChar isEqualToString:@"A"])
        {
            self.nodeStatus = OONodeStatusDisconected;
        }
        
        if ([self respondsToSelector:@selector(refreshCell)]) {
            [self performSelectorOnMainThread:@selector(refreshCell) withObject:nil waitUntilDone:NO];
        }
    }
    [oppPool drain];
}

@end

@implementation NSObject (OOTableCellDictKey)

static char zombieReleaserKey;

- (void)runAtDealloc:(id)arg1
{
    OOZombieDictKey *zombieReleaser = objc_getAssociatedObject(self, &zombieReleaserKey);
    
    if (zombieReleaser == nil) {
        zombieReleaser = [[OOZombieDictKey alloc] init];          
        zombieReleaser.parent = self;
        objc_setAssociatedObject(self, &zombieReleaserKey, zombieReleaser, OBJC_ASSOCIATION_RETAIN);
        [zombieReleaser release];
    }
    [zombieReleaser addDictKey:arg1];
}

@end