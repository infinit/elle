//
//  InfinitCrypto.h
//  Infinit
//
//  Created by Simon Peccaud on 8/2/12.
//  Copyright (c) 2012 infinit.io. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface InfinitCrypto : NSObject
+ (NSString *)cryptEmail:(NSString*) email Password: (NSString*) pass;
+ (NSString *)sha256: (NSString *)str;
@end
