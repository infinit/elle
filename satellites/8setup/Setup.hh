//
// ---------- header ----------------------------------------------------------
//
// project       8setup
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8setup/Setup.hh
//
// created       julien quintard   [sat mar 27 08:37:14 2010]
// updated       julien quintard   [mon may 10 09:55:08 2010]
//

#ifndef SETUP_SETUP_HH
#define SETUP_SETUP_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <lune/Lune.hh>
#include <etoile/Etoile.hh>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
# include <sys/types.h>
# include <dirent.h>
# include <unistd.h>
#include <elle/idiom/Open.hh>

namespace application
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the 8setup application.
  ///
  class Setup
  {
  public:
    //
    // enumerations
    //
    enum Operation
      {
	OperationUnknown = 0,

	OperationInitialize,
	OperationClean,
	OperationCheck
      };

    //
    // static methods
    //
    static elle::Status		Empty(const elle::String&);

    static elle::Status		Initialize();
    static elle::Status		Clean();
    static elle::Status		Check();
  };

}

#endif
