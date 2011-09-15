//
// ---------- header ----------------------------------------------------------
//
// project       8passport
//
// license       infinit
//
// author        julien quintard   [thu aug 11 15:16:11 2011]
//

#ifndef PASSPORT_PASSPORT_HH
#define PASSPORT_PASSPORT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <lune/Lune.hh>
#include <etoile/Etoile.hh>
#include <hole/Hole.hh>

namespace application
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the 8passport application.
  ///
  class Passport
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
    static elle::Status		Create();
    static elle::Status		Destroy();
    static elle::Status		Information();
  };

}

#endif
