//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [tue may 31 10:32:37 2011]
//

#ifndef HORIZON_MACOSX_MACOSX_HH
#define HORIZON_MACOSX_MACOSX_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/types.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Dictionary.hh>

#include <elle/idiom/Close.hh>
# include <sys/types.h>
# include <pwd.h>
#include <elle/idiom/Open.hh>

namespace horizon
{
  ///
  /// this namespace contains all the elements related to the MacOS X horizon
  /// i.e interface between the kernel and Infinit.
  ///
  namespace macosx
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains the general-purpose information related to the
    /// MacOS X horizon.
    ///
    class MacOSX
    {
    public:
      //
      // static methods
      //
      static elle::Status       Initialize();
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

#include <horizon/macosx/Crib.hh>
#include <horizon/macosx/Crux.hh>
#include <horizon/macosx/Event.hh>
#include <horizon/macosx/FUSE.hh>
#include <horizon/macosx/FUker.hh>
#include <horizon/macosx/Handle.hh>
#include <horizon/macosx/Janitor.hh>

#endif
