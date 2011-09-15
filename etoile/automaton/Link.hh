//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri aug 14 18:57:08 2009]
//

#ifndef ETOILE_AUTOMATON_ETOILE_HH
#define ETOILE_AUTOMATON_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/Link.hh>

#include <etoile/path/Way.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for managing directories.
    ///
    class Link
    {
    public:
      //
      // static methods
      //
      static elle::Status	Create(gear::Link&);
      static elle::Status	Load(gear::Link&,
				     const nucleus::Location&);

      static elle::Status	Bind(gear::Link&,
				     const path::Way&);
      static elle::Status	Resolve(gear::Link&,
					path::Way&);

      static elle::Status	Store(gear::Link&);
      static elle::Status	Destroy(gear::Link&);
    };

  }
}

#endif
