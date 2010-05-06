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
// updated       julien quintard   [tue may  4 18:54:42 2010]
//

#ifndef PASSPORT_PASSPORT_HH
#define PASSPORT_PASSPORT_HH

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
	OperationUnset
      };

    //
    // static methods
    //
    static Status	Create(const String&);
    static Status	Destroy(const String&);

    static Status	Set(const String&,
			    const String&,
			    const String&);
    static Status	Get(const String&,
			    const String&);
    static Status	Unset(const String&,
			      const String&);
  };

}

#endif
