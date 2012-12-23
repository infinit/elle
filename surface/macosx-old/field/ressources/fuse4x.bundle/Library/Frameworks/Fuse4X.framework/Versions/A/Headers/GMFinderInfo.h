// ================================================================
// Copyright (c) 2007, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above
//   copyright notice, this list of conditions and the following disclaimer
//   in the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of Google Inc. nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ================================================================
//
//  GMFinderInfo.h
//  Fuse4X
//
//  Created by ted on 12/29/07.
//

/*!
 * @header GMFinderInfo
 *
 * A utility class to construct raw data for FinderInfo.
 *
 * In OS 10.4, the FinderInfo for a file may be present in an AppleDouble (._)
 * file that is associated with the file. In 10.5+, the FinderInfo is present in
 * the com.apple.FinderInfo extended attribute on a file.
 */

#import <Foundation/Foundation.h>

#define GM_EXPORT __attribute__((visibility("default")))

/*!
 * @class
 * @discussion This class can be used to construct raw NSData for FinderInfo.
 * For more information about FinderInfo and what it can contain, see
 * the CarbonCore/Finder.h header file.
 */
GM_EXPORT @interface GMFinderInfo : NSObject {
 @private
  UInt16 flags_;
  UInt16 extendedFlags_;
  OSType typeCode_;
  OSType creatorCode_;
}

/*! @abstract Returns an autorleased GMFinderInfo */
+ (GMFinderInfo *)finderInfo;

/*!
 * @abstract Sets FinderInfo flags.
 * @discussion See CarbonCore/Finder.h for the set of flags.
 * @param flags OR'd set of valid Finder flags.
 */
- (void)setFlags:(UInt16)flags;

/*!
 * @abstract Sets FinderInfo extended flags.
 * @discussion See CarbonCore/Finder.h for the set of extended flags.
 * @param flags OR'd set of valid Finder extended flags.
 */
- (void)setExtendedFlags:(UInt16)extendedFlags;

/*!
 * @abstract Sets FinderInfo four-char type code.
 * @param typeCode The four-char type code to set.
 */
- (void)setTypeCode:(OSType)typeCode;

/*!
 * @abstract Sets FinderInfo four-char creator code.
 * @param typeCode The four-char creator code to set.
 */
- (void)setCreatorCode:(OSType)creatorCode;

/*!
 * @abstract Constucts the raw data for the FinderInfo.
 * @result NSData for the FinderInfo based on the current settings.
 */
- (NSData *)data;

// -- Deprecated Convenience Methods --

/*!
 * @abstract NSData for a FinderInfo with the given flags.
 * @param flags OR'd set of FinderInfo flags.
 * @result NSData of the FinderInfo.
 * @deprecated This method is deprecated.
 */
+ (NSData *)finderInfoWithFinderFlags:(UInt16)flags;

// Suitable only for files (not directories), this version allows you to
// specify type and creator four-char-codes in addition to the flags.
/*!
 * @abstract NSData for a FinderInfo with the given flags, type, and creator.
 * @param flags OR'd set of FinderInfo flags.
 * @param typeCode The four-char type code.
 * @param creatorCode The four-char creator code.
 * @result NSData of the FinderInfo.
 * @deprecated This method is deprecated.
 */
+ (NSData *)finderInfoWithFinderFlags:(UInt16)flags
                             typeCode:(OSType)typeCode
                          creatorCode:(OSType)creatorCode;

@end

#undef GM_EXPORT
