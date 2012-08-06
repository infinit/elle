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
//  GMUserFileSystem.h
//
//  Created by ted on 12/29/07.
//  Based on FUSEFileSystem originally by alcor.
//

/*!
 * @header GMUserFileSystem
 *
 * Contains the class and delegate methods used to create a user space file
 * system. Typical use would be to instantiate a
 * @link GMUserFileSystem GMUserFileSystem @/link instance, providing a delegate
 * that implements the core methods of the file system. The GMUserFileSystem
 * object can then be mounted at a specified path and will pass on file system
 * operations to its delegate until it is unmounted.
 */

#import <Foundation/Foundation.h>

// See "64-bit Class and Instance Variable Access Control"
#define GM_EXPORT __attribute__((visibility("default")))

@class GMUserFileSystemInternal;

/*!
 * @class
 * @discussion This class controls the life cycle of a user space file system.
 * The GMUserFileSystem is typically instantiated with a delegate that will
 * serve file system operations. The delegate needs to implement some or all of
 * the methods in the GMUserFileSystemOperations informal protocol. It may also
 * implement methods from the GMUserFileSystemLifecycle and
 * GMUserFileSystemResourceForks protocols as necessary.<br>
 *
 * After instantiating a GMUserFileSystem with an appropriate delegate, call
 * mountAtPath:withOptions: to mount the file system. A call to unmount or an
 * external umount operation will unmount the file system. If the delegate
 * implements methods from the GMUserFileSystemLifecycle informal protocol then
 * these will be called just before mount and unmount. In addition, the
 * GMUserFileSystem class will post mount and unmount notifications to the
 * default notification center. Since the underlying GMUserFileSystem
 * implementation is multi-threaded, you should assume that notifications will
 * not be posted on the main thread. The object will always be the
 * GMUserFileSystem* and the userInfo will always contain at least the
 * kGMUserFileSystemMountPathkey.<br>
 *
 * The best way to get started with GMUserFileSystem is to look at some example
 * file systems that use Fuse4X.framework. See the example file systems found
 * <a href="http://macfuse.googlecode.com/svn/trunk/filesystems-objc/">here</a>.
 */
GM_EXPORT @interface GMUserFileSystem : NSObject {
 @private
  GMUserFileSystemInternal* internal_;
}

/*!
 * @abstract Initialize the user space file system.
 * @discussion The file system delegate should implement some or all of the
 * GMUserFileSystemOperations informal protocol. You should only specify YES
 * for isThreadSafe if your file system delegate is thread safe with respect to
 * file system operations. That implies that it implements proper file system
 * locking so that multiple operations on the same file can be done safely.
 * @param delegate The file system delegate; implements the file system logic.
 * @param isThreadSafe Is the file system delegate thread safe?
 * @result A GMUserFileSystem instance.
 */
- (id)initWithDelegate:(id)delegate isThreadSafe:(BOOL)isThreadSafe;

/*!
 * @abstract Set the file system delegate.
 * @param delegate The delegate to use from now on for this file system.
 */
- (void)setDelegate:(id)delegate;

/*!
 * @abstract Get the file system delegate.
 * @result The file system delegate.
 */
- (id)delegate;

/*!
 * @abstract Mount the file system at the given path.
 * @discussion Mounts the file system at mountPath with the given set of options.
 * The set of available options can be found on the
 * <a href="http://code.google.com/p/macfuse/wiki/OPTIONS">options</a> wiki page.
 * For example, to turn on debug output add \@"debug" to the options NSArray.
 * If the mount succeeds, then a kGMUserFileSystemDidMount notification is posted
 * to the default noification center. If the mount fails, then a
 * kGMUserFileSystemMountFailed notification will be posted instead.
 * @param mountPath The path to mount on, e.g. /Volumes/MyFileSystem
 * @param options The set of mount time options to use.
 */
- (void)mountAtPath:(NSString *)mountPath
        withOptions:(NSArray *)options;

/*!
 * @abstract Mount the file system at the given path with advanced options.
 * @discussion Mounts the file system at mountPath with the given set of options.
 * This is an advanced version of @link mountAtPath:withOptions: mountAtPath:withOptions @/link
 * You can use this to mount from a command-line program as follows:<ul>
 *  <li>For an app, use: shouldForeground=YES, detachNewThread=YES
 *  <li>For a daemon: shouldForeground=NO, detachNewThread=NO
 *  <li>For debug output: shouldForeground=YES, detachNewThread=NO
 *  <li>For a daemon+runloop:  shouldForeground=NO, detachNewThread=YES<br>
 *    - NOTE: I've never tried daemon+runloop; maybe it doesn't make sense</ul>
 * @param mountPath The path to mount on, e.g. /Volumes/MyFileSystem
 * @param options The set of mount time options to use.
 * @param shouldForeground Should the file system thread remain foreground rather
 *        than daemonize? (Recommend: YES)
 * @param detachNewThread Should the file system run in a new thread rather than
 *        the current one? (Recommend: YES)
 */
- (void)mountAtPath:(NSString *)mountPath
        withOptions:(NSArray *)options
   shouldForeground:(BOOL)shouldForeground     // Recommend: YES
    detachNewThread:(BOOL)detachNewThread;     // Recommend: YES

/*!
 * @abstract Unmount the file system.
 * @discussion Unmounts the file system. The kGMUserFileSystemDidUnmount
 * notification will be posted.
 */
- (void)unmount;

@end

#pragma mark Notifications

/*! @group Notifications */

/*! @abstract Error domain for GMUserFileSystem specific errors */
extern NSString* const kGMUserFileSystemErrorDomain;

/*!
 * @abstract Key in notification dictionary for mount path
 * @discussion The value will be an NSString that is the mount path.
 */
extern NSString* const kGMUserFileSystemMountPathKey;

/*! @abstract Key in notification dictionary for an error */
extern NSString* const kGMUserFileSystemErrorKey;

/*!
 * @abstract Notification sent when the mountAtPath operation fails.
 * @discussion The userInfo will contain an kGMUserFileSystemErrorKey with an
 * NSError* that describes the error.
 */
extern NSString* const kGMUserFileSystemMountFailed;

/*! @abstract Notification sent after the filesystem is successfully mounted. */
extern NSString* const kGMUserFileSystemDidMount;

/*! @abstract Notification sent after the filesystem is successfully unmounted. */
extern NSString* const kGMUserFileSystemDidUnmount;

#pragma mark -

#pragma mark GMUserFileSystem Delegate Protocols

// The GMUserFileSystem's delegate can implement any of the below protocols.
// In most cases you can selectively choose which methods of a protocol to
// implement.

/*!
 * @category
 * @discussion Optional delegate operations that get called as part of a file
 * system's life cycle.
 */
@interface NSObject (GMUserFileSystemLifecycle)

/*! @abstract Called just before the mount of the file system occurs. */
- (void)willMount;

/*! @abstract Called just before an unmount of the file system will occur. */
- (void)willUnmount;

@end

/*!
 * @category
 * @discussion The core set of file system operations the delegate must implement.
 * Unless otherwise noted, they typically should behave like the NSFileManager
 * equivalent. However, the error codes that they return should correspond to
 * the BSD-equivalent call and be in the NSPOSIXErrorDomain.<br>
 *
 * For a read-only filesystem, you can typically pick-and-choose which methods
 * to implement.  For example, a minimal read-only filesystem might implement:<ul>
 *   - (NSArray *)contentsOfDirectoryAtPath:(NSString *)path
 *                                    error:(NSError **)error;<br>
 *   - (NSDictionary *)attributesOfItemAtPath:(NSString *)path
 *                                   userData:(id)userData
 *                                      error:(NSError **)error;<br>
 *   - (NSData *)contentsAtPath:(NSString *)path;</ul>
 * For a writeable filesystem, the Finder can be quite picky unless the majority
 * of these methods are implemented. However, you can safely skip hard-links,
 * symbolic links, and extended attributes.
 */
@interface NSObject (GMUserFileSystemOperations)

#pragma mark Directory Contents

/*!
 * @abstract Returns directory contents at the specified path.
 * @discussion Returns an array of NSString containing the names of files and
 * sub-directories in the specified directory.
 * @seealso man readdir(3)
 * @param path The path to a directory.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result An array of NSString or nil on error.
 */
- (NSArray *)contentsOfDirectoryAtPath:(NSString *)path error:(NSError **)error;

#pragma mark Getting and Setting Attributes

/*!
 * @abstract Returns attributes at the specified path.
 * @discussion
 * Returns a dictionary of attributes at the given path. It is required to
 * return at least the NSFileType attribute. You may omit the NSFileSize
 * attribute if contentsAtPath: is implemented, although this is less efficient.
 * The following keys are currently supported (unknown keys are ignored):<ul>
 *   <li>NSFileType [Required]
 *   <li>NSFileSize [Recommended]
 *   <li>NSFileModificationDate
 *   <li>NSFileReferenceCount
 *   <li>NSFilePosixPermissions
 *   <li>NSFileOwnerAccountID
 *   <li>NSFileGroupOwnerAccountID
 *   <li>NSFileSystemFileNumber             (64-bit on 10.5+)
 *   <li>NSFileCreationDate                 (if supports extended dates)
 *   <li>kGMUserFileSystemFileBackupDateKey (if supports extended dates)
 *   <li>kGMUserFileSystemFileChangeDateKey
 *   <li>kGMUserFileSystemFileAccessDateKey
 *   <li>kGMUserFileSystemFileFlagsKey
 *   <li>kGMUserFileSystemFileSizeInBlocksKey</ul>
 *
 * If this is the fstat variant and userData was supplied in openFileAtPath: or
 * createFileAtPath: then it will be passed back in this call.
 *
 * @seealso man stat(2), fstat(2)
 * @param path The path to the item.
 * @param userData The userData corresponding to this open file or nil.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result A dictionary of attributes or nil on error.
 */
- (NSDictionary *)attributesOfItemAtPath:(NSString *)path
                                userData:(id)userData
                                   error:(NSError **)error;

/*!
 * @abstract Returns file system attributes.
 * @discussion
 * Returns a dictionary of attributes for the file system.
 * The following keys are currently supported (unknown keys are ignored):<ul>
 *   <li>NSFileSystemSize
 *   <li>NSFileSystemFreeSize
 *   <li>NSFileSystemNodes
 *   <li>NSFileSystemFreeNodes
 *   <li>kGMUserFileSystemVolumeSupportsExtendedDatesKey
 *   <li>kGMUserFileSystemVolumeMaxFilenameLengthKey
 *   <li>kGMUserFileSystemVolumeFileSystemBlockSizeKey</ul>
 *
 * @seealso man statvfs(3)
 * @param path A path on the file system (it is safe to ignore this).
 * @param error Should be filled with a POSIX error in case of failure.
 * @result A dictionary of attributes for the file system.
 */
- (NSDictionary *)attributesOfFileSystemForPath:(NSString *)path
                                          error:(NSError **)error;

/*!
 * @abstract Set attributes at the specified path.
 * @discussion
 * Sets the attributes for the item at the specified path. The following keys
 * may be present (you must ignore unknown keys):<ul>
 *   <li>NSFileSize
 *   <li>NSFileOwnerAccountID
 *   <li>NSFileGroupOwnerAccountID
 *   <li>NSFilePosixPermissions
 *   <li>NSFileModificationDate
 *   <li>NSFileCreationDate                  (if supports extended dates)
 *   <li>kGMUserFileSystemFileBackupDateKey  (if supports extended dates)
 *   <li>kGMUserFileSystemFileChangeDateKey
 *   <li>kGMUserFileSystemFileAccessDateKey
 *   <li>kGMUserFileSystemFileFlagsKey</ul>
 *
 * If this is the f-variant and userData was supplied in openFileAtPath: or
 * createFileAtPath: then it will be passed back in this call.
 *
 * @seealso man truncate(2), chown(2), chmod(2), utimes(2), chflags(2),
 *              ftruncate(2), fchown(2), fchmod(2), futimes(2), fchflags(2)
 * @param attributes The attributes to set.
 * @param path The path to the item.
 * @param userData The userData corresponding to this open file or nil.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result YES if the attributes are successfully set.
 */
- (BOOL)setAttributes:(NSDictionary *)attributes
         ofItemAtPath:(NSString *)path
             userData:(id)userData
                error:(NSError **)error;

#pragma mark File Contents

/*!
 * @abstract Returns file contents at the specified path.
 * @discussion Returns the full contents at the given path. Implementation of
 * this delegate method is recommended only by very simple file systems that are
 * not concerned with performance. If contentsAtPath is implemented then you can
 * skip open/release/read.
 * @param path The path to the file.
 * @result The contents of the file or nil if a file does not exist at path.
 */
- (NSData *)contentsAtPath:(NSString *)path;

/*!
 * @abstract Opens the file at the given path for read/write.
 * @discussion This will only be called for existing files. If the file needs
 * to be created then createFileAtPath: will be called instead.
 * @seealso man open(2)
 * @param path The path to the file.
 * @param mode The open mode for the file (e.g. O_RDWR, etc.)
 * @param userData Out parameter that can be filled in with arbitrary user data.
 *        The given userData will be retained and passed back in to delegate
 *        methods that are acting on this open file.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result YES if the file was opened successfully.
 */
- (BOOL)openFileAtPath:(NSString *)path
                  mode:(int)mode
              userData:(id *)userData
                 error:(NSError **)error;

/*!
 * @abstract Called when an opened file is closed.
 * @discussion If userData was provided in the corresponding openFileAtPath: call
 * then it will be passed in userData and released after this call completes.
 * @seealso man close(2)
 * @param path The path to the file.
 * @param userData The userData corresponding to this open file or nil.
 */
- (void)releaseFileAtPath:(NSString *)path userData:(id)userData;

/*!
 * @abstract Reads data from the open file at the specified path.
 * @discussion Reads data from the file starting at offset into the provided
 * buffer and returns the number of bytes read. If userData was provided in the
 * corresponding openFileAtPath: or createFileAtPath: call then it will be
 * passed in.
 * @seealso man pread(2)
 * @param path The path to the file.
 * @param userData The userData corresponding to this open file or nil.
 * @param buffer Byte buffer to read data from the file into.
 * @param size The size of the provided buffer.
 * @param offset The offset in the file from which to read data.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result The number of bytes read or -1 on error.
 */
- (int)readFileAtPath:(NSString *)path
             userData:(id)userData
               buffer:(char *)buffer
                 size:(size_t)size
               offset:(off_t)offset
                error:(NSError **)error;

/*!
 * @abstract Writes data to the open file at the specified path.
 * @discussion Writes data to the file starting at offset from the provided
 * buffer and returns the number of bytes written. If userData was provided in
 * the corresponding openFileAtPath: or createFileAtPath: call then it will be
 * passed in.
 * @seealso man pwrite(2)
 * @param path The path to the file.
 * @param userData The userData corresponding to this open file or nil.
 * @param buffer Byte buffer containing the data to write to the file.
 * @param size The size of the provided buffer.
 * @param offset The offset in the file to write data.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result The number of bytes written or -1 on error.
 */
- (int)writeFileAtPath:(NSString *)path
              userData:(id)userData
                buffer:(const char *)buffer
                  size:(size_t)size
                offset:(off_t)offset
                 error:(NSError **)error;

/*!
 * @abstract Atomically exchanges data between files.
 * @discussion  Called to atomically exchange file data between path1 and path2.
 * @seealso man exchangedata(2)
 * @param path1 The path to the file.
 * @param path2 The path to the other file.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result YES if data was exchanged successfully.
 */
- (BOOL)exchangeDataOfItemAtPath:(NSString *)path1
                  withItemAtPath:(NSString *)path2
                           error:(NSError **)error;

#pragma mark Creating an Item

/*!
 * @abstract Creates a directory at the specified path.
 * @discussion  The attributes may contain keys similar to setAttributes:.
 * @seealso man mkdir(2)
 * @param path The directory path to create.
 * @param attributes Set of attributes to apply to the newly created directory.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result YES if the directory was successfully created.
 */
- (BOOL)createDirectoryAtPath:(NSString *)path
                   attributes:(NSDictionary *)attributes
                        error:(NSError **)error;

/*!
 * @abstract Creates and opens a file at the specified path.
 * @discussion  This should create and open the file at the same time. The
 * attributes may contain keys similar to setAttributes:.
 * @seealso man creat(2)
 * @param path The path of the file to create.
 * @param attributes Set of attributes to apply to the newly created file.
 * @param userData Out parameter that can be filled in with arbitrary user data.
 *        The given userData will be retained and passed back in to delegate
 *        methods that are acting on this open file.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result YES if the directory was successfully created.
 */
- (BOOL)createFileAtPath:(NSString *)path
              attributes:(NSDictionary *)attributes
                userData:(id *)userData
                   error:(NSError **)error;

#pragma mark Moving an Item

/*!
 * @abstract Moves or renames an item.
 * @discussion Move, also known as rename, is one of the more difficult file
 * system methods to implement properly. Care should be taken to handle all
 * error conditions and return proper POSIX error codes.
 * @seealso man rename(2)
 * @param source The source file or directory.
 * @param destination The destination file or directory.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result YES if the move was successful.
 */
- (BOOL)moveItemAtPath:(NSString *)source
                toPath:(NSString *)destination
                 error:(NSError **)error;

#pragma mark Removing an Item

/*!
 * @abstract Remove the directory at the given path.
 * @discussion Unlike NSFileManager, this should not recursively remove
 * subdirectories. If this method is not implemented, then removeItemAtPath
 * will be called even for directories.
 * @seealso man rmdir(2)
 * @param path The directory to remove.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result YES if the directory was successfully removed.
 */
- (BOOL)removeDirectoryAtPath:(NSString *)path error:(NSError **)error;

/*!
 * @abstract Removes the item at the given path.
 * @discussion This should not recursively remove subdirectories. If
 * removeDirectoryAtPath is implemented, then that will be called instead of
 * this selector if the item is a directory.
 * @seealso man unlink(2), rmdir(2)
 * @param path The path to the item to remove.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result YES if the item was successfully removed.
 */
- (BOOL)removeItemAtPath:(NSString *)path error:(NSError **)error;

#pragma mark Linking an Item

/*!
 * @abstract Creates a hard link.
 * @seealso man link(2)
 * @param path The path for the created hard link.
 * @param otherPath The path that is the target of the created hard link.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result YES if the hard link was successfully created.
 */
- (BOOL)linkItemAtPath:(NSString *)path
                toPath:(NSString *)otherPath
                 error:(NSError **)error;

#pragma mark Symbolic Links

/*!
 * @abstract Creates a symbolic link.
 * @seealso man symlink(2)
 * @param path The path for the created symbolic link.
 * @param otherPath The path that is the target of the symbolic link.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result YES if the symbolic link was successfully created.
 */
- (BOOL)createSymbolicLinkAtPath:(NSString *)path
             withDestinationPath:(NSString *)otherPath
                           error:(NSError **)error;

/*!
 * @abstract Reads the destination of a symbolic link.
 * @seealso man readlink(2)
 * @param path The path to the specified symlink.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result The destination path of the symbolic link or nil on error.
 */
- (NSString *)destinationOfSymbolicLinkAtPath:(NSString *)path
                                        error:(NSError **)error;

#pragma mark Extended Attributes

/*!
 * @abstract Returns the names of the extended attributes at the specified path.
 * @discussion If there are no extended attributes at this path, then return an
 * empty array. Return nil only on error.
 * @seealso man listxattr(2)
 * @param path The path to the specified file.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result An NSArray of extended attribute names or nil on error.
 */
- (NSArray *)extendedAttributesOfItemAtPath:path
                                      error:(NSError **)error;

/*!
 * @abstract Returns the contents of the extended attribute at the specified path.
 * @seealso man getxattr(2)
 * @param name The name of the extended attribute.
 * @param path The path to the specified file.
 * @param position The offset within the attribute to read from.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result The data corresponding to the attribute or nil on error.
 */
- (NSData *)valueOfExtendedAttribute:(NSString *)name
                        ofItemAtPath:(NSString *)path
                            position:(off_t)position
                               error:(NSError **)error;

/*!
 * @abstract Writes the contents of the extended attribute at the specified path.
 * @seealso man setxattr(2)
 * @param name The name of the extended attribute.
 * @param path The path to the specified file.
 * @param value The data to write.
 * @param position The offset within the attribute to write to
 * @param options Options (see setxattr man page).
 * @param error Should be filled with a POSIX error in case of failure.
 * @result YES if the attribute was successfully written.
 */
- (BOOL)setExtendedAttribute:(NSString *)name
                ofItemAtPath:(NSString *)path
                       value:(NSData *)value
                    position:(off_t)position
                     options:(int)options
                       error:(NSError **)error;

/*!
 * @abstract Removes the extended attribute at the specified path.
 * @seealso man removexattr(2)
 * @param name The name of the extended attribute.
 * @param path The path to the specified file.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result YES if the attribute was successfully removed.
 */
- (BOOL)removeExtendedAttribute:(NSString *)name
                   ofItemAtPath:(NSString *)path
                          error:(NSError **)error;

@end


/*!
 * @category
 * @discussion Implementing any GMUserFileSystemResourceForks method turns on
 * automatic handling of FinderInfo and ResourceForks. In 10.5 and later these
 * are provided via extended attributes while in 10.4 we use "._" files.
 * Typically, it only makes sense to use these for a read-only file system.
 */
@interface NSObject (GMUserFileSystemResourceForks)

/*!
 * @abstract Returns FinderInfo attributes at the specified path.
 * @discussion Returns a dictionary of FinderInfo attributes at the given path.
 * Return nil or a dictionary with no relevant keys if there is no FinderInfo
 * data. If a custom icon is desired, then use Finder flags with the
 * kHasCustomIcon bit set (preferred) and/or the
 * kGMUserFileSystemCustonIconDataKey, and don't forget to implement
 * resourceAttributesAtPath:error:.
 *
 * The following keys are currently supported (unknown keys are ignored):<ul>
 *   <li>NSFileHFSTypeCode
 *   <li>NSFileHFSCreatorCode
 *   <li>kGMUserFileSystemFinderFlagsKey (NSNumber Uint16 Finder flags)
 *   <li>kGMUserFileSystemFinderExtendedFlagsKey (NSNumber Uint16)
 *   <li>kGMUserFileSystemCustomIconDataKey [Raw .icns file NSData]
 *   <li>TODO: kGMUserFileSystemLabelNumberKey   (NSNumber)</ul>
 * @seealso man getxattr(2)
 * @param path The path to the item.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result A dictionary of FinderInfo attributes or nil on error.
 */
- (NSDictionary *)finderAttributesAtPath:(NSString *)path
                                   error:(NSError **)error;

/*!
 * @abstract Returns ResourceFork attributes at the specified path.
 * @discussion Return nil or a dictionary with no relevant keys if there is no
 * resource fork data.
 *
 * The following keys are currently supported (unknown keys are ignored):<ul>
 *   <li>kGMUserFileSystemCustomIconDataKey [Raw .icns file NSData]
 *   <li>kGMUserFileSystemWeblocURLkey [NSURL, only valid for .webloc files]</ul>
 * @seealso man getxattr(2)
 * @param path The path to the item.
 * @param error Should be filled with a POSIX error in case of failure.
 * @result A dictionary of ResourceFork attributes or nil on error.
 */
- (NSDictionary *)resourceAttributesAtPath:(NSString *)path
                                     error:(NSError **)error;

@end

#pragma mark Additional Item Attribute Keys

/*! @group Additional Item Attribute Keys */

/*!
 * @abstract File flags.
 * @discussion The value should be an NSNumber* with uint32 value that is the
 * file st_flags (man 2 stat).
 */
extern NSString* const kGMUserFileSystemFileFlagsKey;

/*!
 * @abstract File access date.
 * @discussion The value should be an NSDate that is the last file access
 * time. See st_atimespec (man 2 stat).
 */
extern NSString* const kGMUserFileSystemFileAccessDateKey;

/*!
 * @abstract File status change date.
 * @discussion The value should be an NSDate that is the last file status change
 * time. See st_ctimespec (man 2 stat).
 */
extern NSString* const kGMUserFileSystemFileChangeDateKey;

/*!
 * @abstract  For file backup date.
 * @discussion The value should be an NSDate that is the backup date.
 */
extern NSString* const kGMUserFileSystemFileBackupDateKey;

/*! 
 * @abstract  For file size in 512 byte blocks. 
 * @discussion The value should be an NSNumber that is the file size in 512 byte 
 * blocks. It is ignored unless the file system is mounted with option \@"sparse".
 */
extern NSString* const kGMUserFileSystemFileSizeInBlocksKey;

#pragma mark Additional Volume Attribute Keys

/*! @group Additional Volume Attribute Keys */

/*!
 * @abstract Specifies support for extended dates.
 * @discussion The value should be a boolean NSNumber that indicates whether or
 * not the file system supports extended dates such as creation and backup dates.
 */
extern NSString* const kGMUserFileSystemVolumeSupportsExtendedDatesKey;

/*! 
 * @abstract Specifies the maximum filename length in bytes.
 * @discussion The value should be an NSNumber that is the maximum filename length
 * in bytes. If omitted 255 bytes is assumed.
 */	
extern NSString* const kGMUserFileSystemVolumeMaxFilenameLengthKey;	
	
/*! 
 * @abstract Specifies the file system block size in bytes.
 * @discussion The value should be an NSNumber that is the file system block size
 * in bytes. If omitted 4096 bytes is assumed.
 */
extern NSString* const kGMUserFileSystemVolumeFileSystemBlockSizeKey;

#pragma mark Additional Finder and Resource Fork keys

/*! @group Additional Finder and Resource Fork Keys */

/*!
 * @abstract FinderInfo flags.
 * @discussion The value should contain an NSNumber created by OR'ing together
 * Finder flags (e.g. kHasCustomIcon). See CarbonCore/Finder.h.
 */
extern NSString* const kGMUserFileSystemFinderFlagsKey;

/*!
 * @abstract FinderInfo extended flags.
 * @discussion The value should contain an NSNumber created by OR'ing together
 * extended Finder flags. See CarbonCore/Finder.h.
 */
extern NSString* const kGMUserFileSystemFinderExtendedFlagsKey;

/*!
 * @abstract ResourceFork custom icon.
 * @discussion The value should be NSData for a raw .icns file.
 */
extern NSString* const kGMUserFileSystemCustomIconDataKey;

/*!
 * @abstract ResourceFork webloc.
 * @discussion The value should be an NSURL that is the webloc.
 */
extern NSString* const kGMUserFileSystemWeblocURLKey;

#undef GM_EXPORT
