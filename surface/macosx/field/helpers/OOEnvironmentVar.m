//
//  OOEnvironmentVar.m
//  Surface
//
//  Created by Charles Guillot on 06/08/12.
//
//

#import "OOEnvironmentVar.h"

@implementation OOEnvironmentVar

+ (void)setEnvironmentVar {
    NSString* appRessourcePath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"bin"];
    setenv("INFINIT_BINARY_DIR", [appRessourcePath cStringUsingEncoding:NSASCIIStringEncoding] , 1);
}

@end
