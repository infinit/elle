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
// updated       julien quintard   [thu apr 15 13:44:21 2010]
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
      static Status	Lookup(const context::Identifier&,
			       const kernel::Subject&);
      static Status	Consult(const context::Identifier&,
				const kernel::Index&,
				const kernel::Size&);
      static Status	Grant(const context::Identifier&,
			      const kernel::Subject&,
			      const kernel::Permissions&);
      static Status	Update(const context::Identifier&,
			       const kernel::Subject&,
			       const kernel::Permissions&);
      static Status	Block(const context::Identifier&,
			      const kernel::Subject&);
      static Status	Revoke(const context::Identifier&,
			       const kernel::Subject&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Permissions.hh>

#include <etoile/context/Object.hh>
#include <etoile/context/Format.hh>

#include <etoile/components/Access.hh>

#include <etoile/user/User.hh>

#endif
