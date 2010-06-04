//
// ---------- header ----------------------------------------------------------
//
// project       8map
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8map/Map.hh
//
// created       julien quintard   [sat mar 27 08:37:14 2010]
// updated       julien quintard   [mon may 10 17:21:20 2010]
//

#ifndef MAP_MAP_HH
#define MAP_MAP_HH

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
  /// this class implements the 8map application.
  ///
  class Map
  {
  public:
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
    static elle::Status		Add(const Type&,
				    const elle::String&,
				    const elle::Unique&);
    static elle::Status		Remove(const Type&,
				       const elle::String&);
    static elle::Status		Dump(const Type&,
				     const elle::String&);
    static elle::Status		Show(const Type&);
  };

}

#endif
