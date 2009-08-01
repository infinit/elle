//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/system/fuse/System.hh
//
// created       julien quintard   [fri jul 31 22:11:24 2009]
// updated       julien quintard   [sat aug  1 01:23:14 2009]
//

#ifndef PIG_SYSTEM_FUSE_SYSTEM_HH
#define PIG_SYSTEM_FUSE_SYSTEM_HH

//
// ---------- macros ----------------------------------------------------------
//

#define FUSE_USE_VERSION		26

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <pig/system/fuse/Interface.hh>

#include <pig/cache/Cache.hh>

#include <fuse.h>

// XXX
#include <etoile/core/Core.hh>
// XXX

namespace pig
{
  using namespace cache;

  namespace system
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class System
    {
    public:
      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();
      static Status	Main(int,
			     char**);
    };

  }
}

#endif
