//
//  InfinitCrypto.m
//  Infinit
//
//  Created by Simon Peccaud on 8/2/12.
//  Copyright (c) 2012 infinit.io. All rights reserved.
//

#import "InfinitCrypto.h"

static NSString* salt = @"MEGABIET";

@implementation InfinitCrypto

+ (NSString *)cryptEmail:(NSString*) email Password: (NSString*) pass {
    return ([InfinitCrypto sha256:[email stringByAppendingFormat:@"%@%@%@%@", salt, pass, email, salt]]);
}

+ (NSString *)sha256: (NSString *)str
{	
    const char *inputString = [str UTF8String];

    unsigned char hashedChars[32];
    CC_SHA256(inputString , strlen(inputString), hashedChars);
    NSData *hashedData = [NSData dataWithBytes:hashedChars length:32];
    NSString *formattedString = [NSString stringWithFormat:@"%@", hashedData];  // "<XXXXX XXXXX XXXXX XXXXX ...>"ish
    NSString *rawString = [[NSString alloc] init];                               // will be "XXXXXXX..XXX"ish

    rawString = [formattedString stringByReplacingOccurrencesOfString:@" " withString:@""];
    rawString = [rawString stringByReplacingOccurrencesOfString:@">" withString:@""];
    rawString = [rawString stringByReplacingOccurrencesOfString:@"<" withString:@""];

    return rawString;
}

@end
