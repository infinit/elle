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
// updated       julien quintard   [mon may  3 13:50:01 2010]
//

#ifndef ETOILE_WALL_ACCESS_HH
#define ETOILE_WALL_ACCESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/context/Identifier.hh>

#include <etoile/kernel/Subject.hh>
#include <etoile/kernel/Permissions.hh>
#include <etoile/kernel/Index.hh>
#include <etoile/kernel/Size.hh>

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
				       const kernel::Subject&);
      static elle::Status	Consult(const context::Identifier&,
					const kernel::Index&,
					const kernel::Size&);
      static elle::Status	Grant(const context::Identifier&,
				      const kernel::Subject&,
				      const kernel::Permissions&);
      static elle::Status	Update(const context::Identifier&,
				       const kernel::Subject&,
				       const kernel::Permissions&);
      static elle::Status	Block(const context::Identifier&,
				      const kernel::Subject&);
      static elle::Status	Revoke(const context::Identifier&,
				       const kernel::Subject&);
    };

  }
}

#endif
