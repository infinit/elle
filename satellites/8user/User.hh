//
// ---------- header ----------------------------------------------------------
//
// project       8user
//
// license       infinit
//
// author        julien quintard   [sat mar 27 08:37:14 2010]
//

#ifndef USER_USER_HH
#define USER_USER_HH

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
  /// this class implements the 8user application.
  ///
  class User
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
    static elle::Status		Create(const elle::String&);
    static elle::Status		Destroy(const elle::String&);
    static elle::Status		Information(const elle::String&);
  };

}

#endif
