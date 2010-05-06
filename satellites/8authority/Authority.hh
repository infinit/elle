//
// ---------- header ----------------------------------------------------------
//
// project       8authority
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8authority/Authority.hh
//
// created       julien quintard   [sat mar 27 08:37:14 2010]
// updated       julien quintard   [wed may  5 19:52:41 2010]
//

#ifndef AUTHORITY_AUTHORITY_HH
#define AUTHORITY_AUTHORITY_HH

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
  /// this class implements the 8authority application.
  ///
  class Authority
  {
  public:
    //
    // constants
    //
    static const elle::Natural32		Length;

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
