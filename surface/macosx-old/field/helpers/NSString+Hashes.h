//
//  NSString+Hashes.h
//  Surface
//
//  Created by Charles Guillot on 25/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

NSString *NSStringNotNull(NSString *string);

@interface NSString (Hashes)

- (NSString *)md5;
- (NSString *)sha1;
- (NSString *)sha256;
- (NSString *)sha512;

@end