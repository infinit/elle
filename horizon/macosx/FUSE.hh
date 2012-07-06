#ifndef HORIZON_MACOSX_FUSE_HH
# define HORIZON_MACOSX_FUSE_HH

# ifndef FUSE_USE_VERSION
#  define FUSE_USE_VERSION               26
# endif

# include <elle/types.hh>

# include <elle/idiom/Close.hh>
#  define _GNU_SOURCE
#  include <fuse/fuse.h>
# include <elle/idiom/Open.hh>

namespace horizon
{
  namespace macosx
  {

    ///
    /// this class contains everything related to FUSE.
    ///
    /// note that this implementation is based on FUSE4x.
    ///
    class FUSE
    {
    public:
      //
      // static methods
      //
      static elle::Status       Initialize(const struct ::fuse_operations&);
      static elle::Status       Clean();

      //
      // static attributes
      //
      static struct ::fuse_operations   Operations;
    };

  }
}

#endif
