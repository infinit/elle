//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// author        julien quintard   [tue may 31 10:32:37 2011]
//

#ifndef PIG_PIG_HH
#define PIG_PIG_HH

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
/// this namespace contains all the elements related to the PIG - POSIX/Infinit
/// Gateway.
///
namespace pig
{

  ///
  /// this class contains the general-purpose information related to PIG.
  ///
  class PIG
  {
  public:
    //
    // static methods
    //
    static elle::Status		Initialize();
    static elle::Status		Clean();

    static elle::Status		Options();

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

//
// ---------- includes --------------------------------------------------------
//

#include <pig/Crux.hh>
#include <pig/FUSE.hh>
#include <pig/FUker.hh>
#include <pig/InterlacedFUker.hh>
#include <pig/ParallelFUker.hh>
#include <pig/SequentialFUker.hh>

#endif
