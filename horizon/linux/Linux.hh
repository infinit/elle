//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [tue may 31 10:32:37 2011]
//

#ifndef HORIZON_LINUX_LINUX_HH
#define HORIZON_LINUX_LINUX_HH

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

namespace horizon
{
  ///
  /// this namespace contains all the elements related to the Linux horizon i.e
  /// interface between the kernel and Infinit.
  ///
  namespace linux
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains the general-purpose information related to the
    /// Linux horizon.
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

#include <horizon/linux/Crux.hh>
#include <horizon/linux/FUSE.hh>
#include <horizon/linux/FUker.hh>
#include <horizon/linux/InterlacedFUker.hh>
#include <horizon/linux/ParallelFUker.hh>
#include <horizon/linux/SequentialFUker.hh>

#endif
