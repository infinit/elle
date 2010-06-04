//
// ---------- header ----------------------------------------------------------
//
// project       8passport
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8passport/Passport.hh
//
// created       julien quintard   [sat mar 27 08:37:14 2010]
// updated       julien quintard   [mon may 10 09:44:33 2010]
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

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
#include <elle/idiom/Open.hh>

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
    static elle::Status		Create(const elle::String&,
				       const elle::String&);
    static elle::Status		Destroy(const elle::String&,
					const elle::String&);
    static elle::Status		Information(const elle::String&,
					    const elle::String&);
  };

}

#endif
