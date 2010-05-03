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
// updated       julien quintard   [sat may  1 16:22:56 2010]
//

#ifndef USER_USER_HH
#define USER_USER_HH

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
	OperationSet,
	OperationGet,
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
