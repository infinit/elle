//
// ---------- header ----------------------------------------------------------
//
// project       8access
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8access/Access.hh
//
// created       julien quintard   [sat mar 27 08:37:14 2010]
// updated       julien quintard   [sun apr 18 19:59:25 2010]
//

#ifndef ACCESS_ACCESS_HH
#define ACCESS_ACCESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <etoile/Etoile.hh>
#include <map/Map.hh>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
#include <elle/idiom/Open.hh>

namespace application
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the 8access application.
  ///
  class Access
  {
  public:
    //
    // constants
    //
    static const String			Path;

    //
    // enumerations
    //
    enum Operation
      {
	OperationUnknown = 0,

	OperationAdd,
	OperationRemove,
	OperationDump,
	OperationShow
      };

    enum Type
      {
	TypeUnknown = 0,

	TypeUser,
	TypeGroup
      };

    //
    // static methods
    //
    static Status	Add(const Type&,
			    const String&,
			    const PublicKey&,
			    const etoile::hole::Address&);
    static Status	Remove(const Type&,
			       const String&);
    static Status	Dump(const Type&,
			     const String&);
    static Status	Show(const Type&);
  };

}

#endif
