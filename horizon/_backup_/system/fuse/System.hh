//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/_backup_/system/fuse/System.hh
//
// created       julien quintard   [fri jul 31 22:11:24 2009]
// updated       julien quintard   [wed mar 31 20:20:45 2010]
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
#include <etoile/Etoile.hh>

#include <pig/system/fuse/Interface.hh>
#include <pig/cache/Cache.hh>

#include <fuse.h>

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
