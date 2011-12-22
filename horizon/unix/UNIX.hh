//
// ---------- header ----------------------------------------------------------
//
// project       facade
//
// license       infinit
//
// author        julien quintard   [tue may 31 10:32:37 2011]
//

#ifndef FACADE_UNIX_UNIX_HH
#define FACADE_UNIX_UNIX_HH

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

///
/// this namespace contains all the elements related to the UNIX facade i.e
/// interface between the kernel and Infinit.
///
namespace facade
{
#undef unix
  ///
  /// XXX
  ///
  namespace unix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains the general-purpose information related to the
    /// UNIX facade.
    ///
    class UNIX
    {
    public:
      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      //
      // static attributes
      //
      struct			Somebody
      {
	static uid_t		UID;
	static gid_t		GID;
      };

      static lune::Dictionary	Dictionary;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <facade/unix/Crux.hh>
#include <facade/unix/FUSE.hh>
#include <facade/unix/FUker.hh>
#include <facade/unix/InterlacedFUker.hh>
#include <facade/unix/ParallelFUker.hh>
#include <facade/unix/SequentialFUker.hh>

#endif
