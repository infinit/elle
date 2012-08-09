//
//  OOManifestItem.m
//  phone
//
//  Created by Charles Guillot on 25/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOManifestItem.h"
#import "OOManifestParser.h"
#import <CommonCrypto/CommonDigest.h>

#define BUFFER_SIZE    4096

@implementation OOManifestItem

@synthesize path, size, md5sum;
@synthesize downloadedSize;
@synthesize parentParser;

- (void)updateWithManager:(NSFileManager*)arg1 absoluteLocalPath:(NSString*)arg2 absoluteRemotePath:(NSString*)arg3
{
    self.downloadedSize = [NSNumber numberWithFloat:0];
    NSURL *destPath = [NSURL URLWithString:[NSString stringWithFormat:@"%@/%@", arg2, self.path]];
    NSURL *sourcePath = [NSURL URLWithString:[NSString stringWithFormat:@"%@/%@", arg3, self.path]];
    if ([arg1 fileExistsAtPath:[destPath absoluteString]]) {
        NSInputStream* iStream = [[NSInputStream alloc] initWithFileAtPath:[destPath absoluteString]];
        [iStream open];
        NSString* oldMD5 = [self base64md5FromStream:iStream];
        NSString* newMD5 = self.md5sum;
        if ([oldMD5 isEqualToString:newMD5]) {
            self.downloadedSize = self.size;
            [[NSNotificationCenter defaultCenter] postNotificationName:OODownloadingNotification
                                                                object:self];
            NSLog(@"%@",@"download not needed");
            return;
        }
    }
    [arg1 createDirectoryAtPath:[[destPath absoluteString]stringByDeletingLastPathComponent] withIntermediateDirectories:YES attributes:nil error:nil];
    NSURLRequest* request = [NSURLRequest requestWithURL:sourcePath];
    NSURLDownload* urlDownload = [[NSURLDownload alloc] initWithRequest:request delegate:self];
    [urlDownload setDestination:[destPath absoluteString] allowOverwrite:YES];
}

- (void)download:(NSURLDownload *)download didFailWithError:(NSError *)error
{ 
    // Inform the user.
    NSLog(@"Download failed! Error - %@ %@",
          [error localizedDescription],
          [[error userInfo] objectForKey:NSURLErrorFailingURLStringErrorKey]);
}

- (void)downloadDidFinish:(NSURLDownload *)download
{
    self.downloadedSize = self.size;
    [[NSNotificationCenter defaultCenter] postNotificationName:OODownloadingNotification 
                                                        object:self];
    NSLog(@"%@",@"downloadDidFinish");
}

-(NSString *)base64md5FromStream:(NSInputStream *)inputStream
{
    if ( [inputStream streamStatus] == NSStreamStatusOpen) {
        CC_MD5_CTX hashObject;
        CC_MD5_Init(&hashObject);
        
        uint8_t buffer[BUFFER_SIZE];
        while ( [inputStream hasBytesAvailable]) {
            NSInteger result = [inputStream read:buffer maxLength:BUFFER_SIZE];
            
            if (result == -1) {
                @throw [NSException exceptionWithName : @"StreamException" reason : @"Unable to properly read stream." userInfo : nil];
            }
            
            CC_MD5_Update(&hashObject, (const void *)buffer, (CC_LONG)result);
        }
        
        unsigned char digest[CC_MD5_DIGEST_LENGTH];
        CC_MD5_Final(digest, &hashObject);
        
        NSData *md5 = [[NSData alloc] initWithBytes:digest length:CC_MD5_DIGEST_LENGTH];
        NSLog(@"hashedData = %@", md5);
        return [md5 hexadecimalString];
    }
    
    return nil;
}
@end

@implementation NSData (NSData_Conversion)

- (NSString *)hexadecimalString {
    /* Returns hexadecimal string of NSData. Empty string if data is empty.   */
    
    const unsigned char *dataBuffer = (const unsigned char *)[self bytes];
    
    if (!dataBuffer)
        return [NSString string];
    
    NSUInteger          dataLength  = [self length];
    NSMutableString     *hexString  = [NSMutableString stringWithCapacity:(dataLength * 2)];
    
    for (int i = 0; i < dataLength; ++i)
        [hexString appendString:[NSString stringWithFormat:@"%02x", (unsigned long)dataBuffer[i]]];
    
    return [NSString stringWithString:hexString];
}

@end
