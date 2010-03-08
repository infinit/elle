//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/wall/Object.hh
//
// created       julien quintard   [fri aug 14 15:36:23 2009]
// updated       julien quintard   [wed mar  3 20:10:06 2010]
//

#ifndef ETOILE_WALL_OBJECT_HH
#define ETOILE_WALL_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/wall/Types.hh>

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
    class Object
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

      static Status	Lock(const Identifier&);
      static Status	Release(const Identifier&);

      static Status	Information(const Identifier&,
				    Metadata&);

      static Status	Rollback(const Identifier&,
				 Version&);

      static Status	Destroy(const Identifier&);

      struct		Attribute
      {
	static Status	Add(const Identifier&,
			    const Name&,
			    const Value&);
	static Status	Remove(const Identifier&,
			       const Name&);
	static Status	Update(const Identifier&,
			       const Name&,
			       const Value&);
      };

      struct		Access
      {
	static Status	List(const Identifier&);
	static Status	Grant(const Identifier&,
			      const Subject&,
			      const Name&,
			      const Permissions&);
	static Status	Revoke(const Identifier&,
			       const Subject&,
			       const Name&);
	static Status	Update(const Identifier&,
			       const Subject&,
			       const Name&,
			       const Permissions&);
	static Status	Block(const Identifier&,
			      const Subject&,
			      const Name&);
      };
    };

  }
}

#endif
