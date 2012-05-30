//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [fri jul 31 22:11:24 2009]
//

#ifndef HORIZON_LINUX_FUSE_HH
#define HORIZON_LINUX_FUSE_HH

//
// ---------- macros ----------------------------------------------------------
//

#ifndef FUSE_USE_VERSION
# define FUSE_USE_VERSION               26
#endif

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/types.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

#include <horizon/linux/FUker.hh>

#include <elle/idiom/Close.hh>
# include <fuse/fuse.h>
#include <elle/idiom/Open.hh>

namespace horizon
{
  namespace linux
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains everything related to FUSE.
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
