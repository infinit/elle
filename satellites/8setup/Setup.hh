//
// ---------- header ----------------------------------------------------------
//
// project       8setup
//
// license       infinit
//
// author        julien quintard   [sat mar 27 08:37:14 2010]
//

#ifndef SETUP_SETUP_HH
#define SETUP_SETUP_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <lune/Lune.hh>
#include <etoile/Etoile.hh>

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
	OperationClean
      };

    //
    // static methods
    //
    static elle::Status		Initialize();
    static elle::Status		Clean();
  };

}

#endif
