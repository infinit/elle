#import <Foundation/Foundation.h>

@interface NSObject (FISwizzle)

+ (BOOL)fi_swizzleMethod:(SEL)origSel_ withMethod:(SEL)altSel_ error:(NSError**)error_;
+ (BOOL)fi_swizzleClassMethod:(SEL)origSel_ withClassMethod:(SEL)altSel_ error:(NSError**)error_;

@end

