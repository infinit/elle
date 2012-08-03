#ifndef OPERATIONS_HH
# define OPERATIONS_HH

# include <elle/system/Platform.hh>

#if defined(HAVE_SETXATTR)
# if defined(INFINIT_MACOSX)
#  define INFINIT_FUSE_COMMANDS_XATTR                                   \
  ((setxattr, (const char*)(const char*)(const char*)                   \
    (size_t)(int)(uint32_t)))                                           \
  ((getxattr, (const char*)(const char*)(char*)(size_t)(uint32_t)))     \
  ((listxattr, (const char*)(char*)(size_t)))                           \
  ((removexattr, (const char*)(const char*)))
# else
#  define INFINIT_FUSE_COMMANDS_XATTR                                   \
  ((setxattr, (const char*)(const char*)(const char*)(size_t)(int)))    \
  ((getxattr, (const char*)(const char*)(char*) (size_t)))              \
  ((listxattr, (const char*)(char*)(size_t)))                           \
  ((removexattr, (const char*)(const char*)))
# endif
#else
# define INFINIT_FUSE_COMMANDS_XATTR
#endif

#if defined(HAVE_STATFS)
# define INFINIT_FUSE_COMMANDS_STATFS                                   \
  ((statfs, (const char*)(struct ::statvfs*)))
#else
# define INFINIT_FUSE_COMMANDS_STATFS
#endif

#define INFINIT_FUSE_COMMANDS                                           \
  ((getattr, (const char*)(struct ::stat*)))                            \
  ((fgetattr, (const char*)(struct ::stat*)                             \
    (struct ::fuse_file_info*)))                                        \
  ((utimens, (const char*)(const timespec2)))                           \
  ((opendir, (const char*)(struct ::fuse_file_info*)))                  \
  ((readdir, (const char*)(void*) (::fuse_fill_dir_t)(off_t)            \
    (struct ::fuse_file_info*)))                                        \
  ((releasedir, (const char*)(struct ::fuse_file_info*)))               \
  ((mkdir, (const char*)(mode_t)))                                      \
  ((rmdir, (const char*)))                                              \
  ((access, (const char*)(int)))                                        \
  ((chmod, (const char*)(mode_t)))                                      \
  ((chown, (const char*)(uid_t)(gid_t)))                                \
  ((symlink, (const char*)(const char*)))                               \
  ((readlink, (const char*)(char*)(size_t)))                            \
  ((create, (const char*)(mode_t)(struct ::fuse_file_info*)))           \
  ((open, (const char*)(struct ::fuse_file_info*)))                     \
  ((write, (const char*)(const char*)(size_t)(off_t)                    \
    (struct ::fuse_file_info*)))                                        \
  ((read, (const char*)(char*)(size_t)(off_t)                           \
    (struct ::fuse_file_info*)))                                        \
  ((truncate, (const char*)(off_t)))                                    \
  ((ftruncate, (const char*)(off_t)(struct ::fuse_file_info*)))         \
  ((release, (const char*)(struct ::fuse_file_info*)))                  \
  ((rename, (const char*)(const char*)))                                \
  ((link, (const char*)(const char*)))                                  \
  ((unlink, (const char*)))                                             \
  INFINIT_FUSE_COMMANDS_XATTR                                           \
  INFINIT_FUSE_COMMANDS_STATFS                                          \

#endif
