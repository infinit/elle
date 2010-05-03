//
// ---------- header ----------------------------------------------------------
//
// project       8universe
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8universe/Universe.hh
//
// created       julien quintard   [sat mar 27 08:37:14 2010]
// updated       julien quintard   [sat may  1 20:46:05 2010]
//

#ifndef UNIVERSE_UNIVERSE_HH
#define UNIVERSE_UNIVERSE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <lune/Lune.hh>
#include <etoile/Etoile.hh>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
#include <elle/idiom/Open.hh>

namespace application
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the 8universe application.
  ///
  class Universe
  {
  public:
    //
    // enumerations
    //
    enum Operation
      {
	OperationUnknown = 0,

	OperationCreate,
	OperationDestroy,
	OperationInformation
      };

    //
    // static methods
    //
    static Status	Create(const String&,
			       const String&);
    static Status	Destroy(const String&);
    static Status	Information(const String&);
  };

}

#endif
