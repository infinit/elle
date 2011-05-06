//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Access.hh
//
// created       julien quintard   [wed mar 31 19:23:49 2010]
// updated       julien quintard   [thu may  5 16:32:15 2011]
//

#ifndef ETOILE_WALL_ACCESS_HH
#define ETOILE_WALL_ACCESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/context/Identifier.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for controlling the access
    /// control mechanism.
    ///
    class Access
    {
    public:
      //
      // static methods
      //
      static elle::Status	Lookup(const context::Identifier&,
				       const nucleus::Subject&);
      static elle::Status	Consult(const context::Identifier&,
					const nucleus::Index&,
					const nucleus::Size&);
      static elle::Status	Grant(const context::Identifier&,
				      const nucleus::Subject&,
				      const nucleus::Permissions&);
      static elle::Status	Update(const context::Identifier&,
				       const nucleus::Subject&,
				       const nucleus::Permissions&);
      static elle::Status	Block(const context::Identifier&,
				      const nucleus::Subject&);
      static elle::Status	Revoke(const context::Identifier&,
				       const nucleus::Subject&);
    };

  }
}

#endif
