//
//  OOManifestParser.h
//  phone
//
//  Created by Charles Guillot on 25/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "OOManifestItem.h"

extern NSString *OODownloadingNotification;

@interface OOManifestParser : NSObject <NSXMLParserDelegate> {
	//This variable will eventually (once the asynchronous event has completed) hold all the RSSItems in the feed
    NSMutableArray *allItems;
    
    //This variable will be used to map properties in the XML to properties in the RSSItem object
    NSMutableArray *propertyMap;
    
    //This variable will be used to build up the data coming back from NSURLConnection
    NSMutableData *receivedData;
    
    //This item will be declared and created each time a new RSS item is encountered in the XML
    OOManifestItem *currentItem;
    
    //This stores the value of the XML element that is currently being processed
    NSMutableString *currentValue;
    
    //File Manager
    NSFileManager *fileManager;
    
    //File URL
    NSString *fileURL;
    
    //Release size
    int releaseSize;
    
    //Release size
    int updatedSize;
    
    //This allows the creating object to know when parsing has completed
    BOOL parsing;
    
    //This internal variable allows the object to know if the current property is inside an item element
    BOOL inItemElement;
}

@property (nonatomic, readonly) NSMutableArray *allItems;
@property (nonatomic, retain)   NSMutableArray *propertyMap;
@property (nonatomic, retain)   NSData *receivedData;
@property (nonatomic, retain)   OOManifestItem *currentItem;
@property (nonatomic, retain)   NSMutableString *currentValue;
@property (nonatomic, retain)   NSFileManager *fileManager;
@property (nonatomic, retain)   NSString *fileURL;
@property                       int releaseSize;
@property                       int updatedSize;
@property BOOL parsing;

//This method kicks off a parse of a URL at a specified string
- (void)startParse:(NSString*)url;
- (void)downloadingNotification:(NSNotification *)notification;
@end