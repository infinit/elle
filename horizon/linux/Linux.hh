//
// ---------- header ----------------------------------------------------------
//
// project       facade
//
// license       infinit
//
// author        julien quintard   [tue may 31 10:32:37 2011]
//

#ifndef FACADE_LINUX_LINUX_HH
#define FACADE_LINUX_LINUX_HH

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
  /// this namespace contains all the elements related to the Linux facade i.e
  /// interface between the kernel and Infinit.
  ///
  namespace linux
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains the general-purpose information related to the
    /// Linux facade.
    ///
    class Linux
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

#include <facade/linux/Crux.hh>
#include <facade/linux/FUSE.hh>
#include <facade/linux/FUker.hh>
#include <facade/linux/InterlacedFUker.hh>
#include <facade/linux/ParallelFUker.hh>
#include <facade/linux/SequentialFUker.hh>

#endif
