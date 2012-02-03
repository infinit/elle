//
// ---------- header ----------------------------------------------------------
//
// project       facade
//
// license       infinit
//
// author        julien quintard   [tue may 31 10:32:37 2011]
//

#ifndef FACADE_MACOSX_MACOSX_HH
#define FACADE_MACOSX_MACOSX_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

#include <elle/idiom/Close.hh>
# include <sys/types.h>
# include <pwd.h>
#include <elle/idiom/Open.hh>

namespace facade
{
  ///
  /// this namespace contains all the elements related to the MacOS X facade
  /// i.e interface between the kernel and Infinit.
  ///
  namespace macosx
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains the general-purpose information related to the
    /// MacOS X facade.
    ///
    class MacOSX
    {
    public:
      //
      // static methods
      //
      static elle::Status       Initialize();
      static elle::Status       Setup();
      static elle::Status       Clean();

      //
      // static attributes
      //
      struct                    Somebody
      {
        static uid_t            UID;
        static gid_t            GID;
      };

      static lune::Dictionary   Dictionary;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <facade/macosx/Crux.hh>
#include <facade/macosx/FUSE.hh>
#include <facade/macosx/FUker.hh>
#include <facade/macosx/InterlacedFUker.hh>
#include <facade/macosx/ParallelFUker.hh>
#include <facade/macosx/SequentialFUker.hh>

#endif
