//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [fri jul 31 22:11:24 2009]
//

#ifndef HORIZON_MACOSX_FUSE_HH
#define HORIZON_MACOSX_FUSE_HH

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
#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

#include <horizon/macosx/FUker.hh>

#include <elle/idiom/Close.hh>
# define _GNU_SOURCE
# include <fuse/fuse.h>
#include <elle/idiom/Open.hh>

namespace horizon
{
  namespace macosx
  {

//
// ---------- classes ---------------------------------------------------------
//

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
      static elle::Status       Initialize(const elle::String&,
                                           const struct ::fuse_operations&);
      static elle::Status       Clean();

      //
      // static attributes
      //
      static elle::String               Mountpoint;

      static struct ::fuse_operations   Operations;
    };

  }
}

#endif
