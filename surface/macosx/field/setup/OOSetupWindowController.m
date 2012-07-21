//
//  OOSetupWindowController.m
//  Surface
//
//  Created by Charles Guillot on 29/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOSetupWindowController.h"

@implementation OOSetupWindowController

+ (OOSetupWindowController *)getInstance {
    
    static OOSetupWindowController *singleton;
    @synchronized(self){
        if (![singleton window]){
            // Le singleton n'a pas encore été instancié
            singleton = [[OOSetupWindowController alloc] init];
        }
        return singleton;
    }
    
}

- (id) init {
    if(self = [super initWithWindowNibName:@"OOSetupWindow"]) {
    }
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

@end
