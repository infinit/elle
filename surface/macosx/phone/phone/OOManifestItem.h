//
//  OOManifestItem.h
//  phone
//
//  Created by Charles Guillot on 25/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface OOManifestItem : NSObject<NSURLDownloadDelegate> {
    NSString    *path;
    NSNumber    *size;
    NSString    *md5sum;
    
    NSNumber    *downloadedSize;
    id          parentParser;
}

@property (nonatomic, retain) NSString *path;
@property (nonatomic, retain) NSNumber *size;
@property (nonatomic, retain) NSString *md5sum;


@property (nonatomic, retain) NSNumber *downloadedSize;
@property (nonatomic, retain) id        parentParser;


- (void)updateWithManager:(NSFileManager*)arg1 absoluteLocalPath:(NSString*)arg2 absoluteRemotePath:(NSString*)arg3;

@end

@interface NSData (NSData_Conversion)

- (NSString *)hexadecimalString;

@end