//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/wall/Directory.hh
//
// created       julien quintard   [fri aug 14 15:36:23 2009]
// updated       julien quintard   [fri aug 14 18:58:55 2009]
//

#ifndef ETOILE_WALL_DIRECTORY_HH
#define ETOILE_WALL_DIRECTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/wall/Types.hh>
#include <etoile/wall/Object.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Directory:
      public Object
    {
    public:
      //
      // static methods
      //
      static Status	Load(const Path&,
			     Identifier&);
      static Status	Create(const Path&,
			       Identifier&);
      static Status	Store(const Identifier&);

      static Status	Destroy(const Identifier&);

      struct		Access
      {
	static Status	Grant(const Identifier&,
			      const Subject&,
			      const Name&,
			      const Permissions&);
	static Status	Update(const Identifier&,
			       const Subject&,
			       const Name&,
			       const Permissions&);
      };
    };

  }
}

#endif
