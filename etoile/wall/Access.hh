//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [wed mar 31 19:23:49 2010]
//

#ifndef ETOILE_WALL_ACCESS_HH
#define ETOILE_WALL_ACCESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/Identifier.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for manipulating the access
    /// control mechanism.
    ///
    class Access
    {
    public:
      //
      // static methods
      //
      static elle::Status	Lookup(const gear::Identifier&,
				       const nucleus::Subject&,
				       nucleus::Record*&);
      static elle::Status	Consult(const gear::Identifier&,
					const nucleus::Index&,
					const nucleus::Size&,
					nucleus::Range<nucleus::Record>&);
      static elle::Status	Grant(const gear::Identifier&,
				      const nucleus::Subject&,
				      const nucleus::Permissions&);
      static elle::Status	Revoke(const gear::Identifier&,
				       const nucleus::Subject&);
    };

  }
}

#endif
