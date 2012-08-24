#ifndef HORIZON_LINUX_CRUX_HH
# define HORIZON_LINUX_CRUX_HH

# ifndef FUSE_USE_VERSION
#  define FUSE_USE_VERSION               26
# endif

# include <elle/types.hh>
# include <elle/system/Platform.hh>

# include <nucleus/neutron/Size.hh>

# include <elle/idiom/Close.hh>
#  include <fuse.h>
#  if defined(HAVE_SETXATTR)
#   include <attr/xattr.h>
#  endif
# include <elle/idiom/Open.hh>

namespace horizon
{
  namespace linux
  {

    ///
    /// this class contains the implementation of the FUSE upcalls.
    ///
    class Crux
    {
    public:
      //
      // constants
      //
      static const nucleus::neutron::Size                Range;

      //
      // callbacks
      //

      // general purpose
      static int
      getattr(const char*,
              struct ::stat*);
      static int
      fgetattr(const char*,
               struct ::stat*,
               struct ::fuse_file_info*);
      static int
      utimens(const char*,
              const struct ::timespec[2]);

      // directory
      static int
      opendir(const char*,
              struct ::fuse_file_info*);
      static int
      readdir(const char*,
              void*,
              ::fuse_fill_dir_t,
              off_t,
              struct ::fuse_file_info*);
      static int
      releasedir(const char*,
                 struct ::fuse_file_info*);
      static int
      mkdir(const char*,
            mode_t);
      static int
      rmdir(const char*);

      // access
      static int
      access(const char*,
             int);
      static int
      chmod(const char*,
            mode_t);
      static int
      chown(const char*,
            uid_t,
            gid_t);

#if defined(HAVE_SETXATTR)
      // attribute
      static int
      setxattr(const char*,
               const char*,
               const char*,
               size_t,
               int);
      static int
      getxattr(const char*,
               const char*,
               char*,
               size_t);
      static int
      listxattr(const char*,
                char*,
                size_t);
      static int
      removexattr(const char*,
                  const char*);
#endif

      // link
      static int
      link(const char* target,
           const char* source);
      static int
      symlink(const char*,
              const char*);
      static int
      readlink(const char*,
               char*,
               size_t);

      // file
      static int
      create(const char*,
             mode_t,
             struct ::fuse_file_info*);
      static int
      open(const char*,
           struct ::fuse_file_info*);
      static int
      write(const char*,
            const char*,
            size_t,
            off_t,
            struct ::fuse_file_info*);
      static int
      read(const char*,
           char*,
           size_t,
           off_t,
           struct ::fuse_file_info*);
      static int
      truncate(const char*,
               off_t);
      static int
      ftruncate(const char*,
                off_t,
                struct ::fuse_file_info*);
      static int
      release(const char*,
              struct ::fuse_file_info*);

      // objects
      static int
      rename(const char*,
             const char*);
      static int
      unlink(const char*);
    };

  }
}

#endif
