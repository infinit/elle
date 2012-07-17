//
//  OOManifestParser.m
//  phone
//
//  Created by Charles Guillot on 25/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "OOManifestParser.h"
#import "OOphone.h"

NSString *OODownloadingNotification = @"OODownloadingNotification";

@implementation OOManifestParser

@synthesize allItems;
@synthesize propertyMap;
@synthesize receivedData;
@synthesize currentItem;
@synthesize currentValue;
@synthesize fileManager;
@synthesize fileURL;
@synthesize parsing;
@synthesize releaseSize;
@synthesize updatedSize;

- (void)startParse:(NSString *)url
{
    self.fileURL = url;
    self.fileManager = [[NSFileManager alloc] init];
    
    //Set the status to parsing
    parsing = true;
    
	//Initialise the receivedData object
    receivedData = [[NSMutableData alloc] init];
    
    //Init all items
    allItems = [[NSMutableArray alloc] init];
    
    //Create the connection with the string URL and kick it off
    NSURLConnection *urlConnection = [NSURLConnection connectionWithRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:url]] delegate:self];
    [urlConnection start];
}


- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
	//Reset the data as this could be fired if a redirect or other response occurs
    [receivedData setLength:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
	//Append the received data each time this is called
    [receivedData appendData:data];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	//Start the XML parser with the delegate pointing at the current object
    NSXMLParser *parser = [[NSXMLParser alloc] initWithData:receivedData];
    [parser setDelegate:self];
    [parser parse];
    
    parsing = false;
}

- (void)parserDidStartDocument:(NSXMLParser *)parser
{
	//Create the property map that will be used to check and populate from elements
    propertyMap = [[NSMutableArray alloc] initWithObjects:@"executable", nil];
    //Clear allItems each time we kick off a new parse
    [allItems removeAllObjects];
}

- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict
{
	//If we find an item element, then ensure that the object knows we are inside it, and that the new item is allocated
    if ([elementName isEqualToString:@"executable"])
    {
        currentItem = [[OOManifestItem alloc] init];
        [currentItem setValuesForKeysWithDictionary:attributeDict];
        currentItem.parentParser = self;
        [allItems addObject:currentItem];
        inItemElement = true;
    }
    if ([elementName isEqualToString:@"release"]) {
        self.releaseSize = [[attributeDict objectForKey:@"size"] intValue];
    }
}

- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName
{
    
    //If we've reached the end of an element then we should the scrap the value regardless of whether we've used it
    currentValue = nil;
}

- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string
{
	//When we find characters inside an element, we can add it to the current value, which is created if it is not initialized at present
    if (!currentValue)
    {
        currentValue = [[NSMutableString alloc] init];
    }
    [currentValue appendString:string];
}

- (void)parserDidEndDocument:(NSXMLParser *)parser {
    [[NSNotificationCenter defaultCenter] postNotificationName:OOUpdateProgessChangedNotification 
                                                        object:self 
                                                      userInfo:[NSDictionary dictionaryWithObject:[NSNumber numberWithFloat:0] 
                                                                                           forKey:@"progress"]];
    [[NSNotificationCenter defaultCenter] addObserver:self 
                                             selector:@selector(downloadingNotification:) 
                                                 name:OODownloadingNotification 
                                               object:nil];
    
    for(OOManifestItem *item in allItems) {
		[item updateWithManager:self.fileManager absoluteLocalPath:[[NSBundle mainBundle] resourcePath] absoluteRemotePath:@"http://download.infinit.im/macosx64"];
    }
}

- (void)downloadingNotification:(NSNotification *)notification {
    int total = 0;
    int downloaded = 0;
    for(OOManifestItem *item in allItems) {
		total += [item.size floatValue];
        downloaded += [item.downloadedSize floatValue];
    }
    float percent = (float)downloaded / total;
    [[NSNotificationCenter defaultCenter] postNotificationName:OOUpdateProgessChangedNotification 
                                                        object:self 
                                                      userInfo:[NSDictionary dictionaryWithObject:[NSNumber numberWithFloat:percent] 
                                                                                           forKey:@"progress"]];
}

@end
