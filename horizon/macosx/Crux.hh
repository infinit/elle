#ifndef HORIZON_MACOSX_CRUX_HH
# define HORIZON_MACOSX_CRUX_HH

# ifndef FUSE_USE_VERSION
#  define FUSE_USE_VERSION               26
# endif

# include <elle/types.hh>
# include <elle/system/platform.hh>

# include <elle/idiom/Close.hh>
#  define _GNU_SOURCE
#  include <fuse.h>
#  if defined(HAVE_SETXATTR)
#   include <sys/xattr.h>
#  endif
# include <elle/idiom/Open.hh>

namespace horizon
{
  namespace macosx
  {

    /// This class contains the implementation of the MacOS X-specific
    /// FUSE upcalls.
    class Crux
    {
    public:
      //
      // callbacks
      //

      // general purpose
      static
      int
      statfs(const char*,
             struct ::statvfs*);

#if defined(HAVE_SETXATTR)
      // attribute
      static
      int
      setxattr(const char*,
               const char*,
               const char*,
               size_t,
               int,
               uint32_t);
      static
      int
      getxattr(const char*,
               const char*,
               char*,
               size_t,
               uint32_t);
#endif
    };

  }
}

#endif
