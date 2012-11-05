//
//  main.m
//  Infinitest
//
//  Created by Simon Peccaud on 3/20/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "InfinitAppDelegate.h"

int main(int argc, char *argv[])
{
    int retVal = 0;
    @autoreleasepool {
        NSString *classString = NSStringFromClass([InfinitAppDelegate class]);
        @try {
            retVal = UIApplicationMain(argc, argv, nil, classString);
        }
        @catch (NSException *exception) {
            NSLog(@"Exception - %@",[exception description]);
            exit(EXIT_FAILURE);
        }
    }
    return retVal;
}