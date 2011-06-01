//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/PIG.hh
//
// created       julien quintard   [tue may 31 10:32:37 2011]
// updated       julien quintard   [wed jun  1 10:01:29 2011]
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
/// gateway.
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
    static elle::Status		Initialize(const elle::String&,
					   const elle::String&,
					   const elle::String&);
    static elle::Status		Clean();

    //
    // static attributes
    //
    static nucleus::Subject*		Subject;

    struct				Somebody
    {
      static uid_t			UID;
      static gid_t			GID;
    };

    static lune::Dictionary		Dictionary;
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <pig/FUSE.hh>

#endif
