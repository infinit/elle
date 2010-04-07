//
// ---------- header ----------------------------------------------------------
//
// project       8user
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8user/User.hh
//
// created       julien quintard   [sat mar 27 08:37:14 2010]
// updated       julien quintard   [sat mar 27 09:19:05 2010]
//

#ifndef USER_USER_HH
#define USER_USER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
#include <elle/idiom/Open.hh>

namespace application
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the 8user application.
  ///
  class User
  {
  public:
    //
    // constants
    //
    static const String			Path;

    //
    // static methods
    //
    static Status	New(const String&);
  };

}

#endif
