//
//  OOUserProfileImage.m
//  FinderPanel
//
//  Created by Charles Guillot on 19/09/12.
//
//

#import "OOUserProfileImage.h"
#import "NSString+Hashes.h"

@implementation OOUserProfileImage

- (id)initWithGravatarEmail:(NSString*)arg1 {
    NSString* md5 = [arg1 md5];
    NSString* strURL = [NSString stringWithFormat:@"http://www.gravatar.com/avatar/%@.jpg?s=128&d=mm", md5];
    NSURL* url = [[NSURL alloc] initWithString:strURL];
    return [self initByReferencingURL:url];
}

@end
