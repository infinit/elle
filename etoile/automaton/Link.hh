//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri aug 14 18:57:08 2009]
//

#ifndef ETOILE_AUTOMATON_LINK_HH
#define ETOILE_AUTOMATON_LINK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/path/Way.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// this class needs to be forwarded declared in order to prevent
    /// conflits.
    ///
    class Link;

  }

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
      static elle::Status       Create(gear::Link&);
      static elle::Status       Load(gear::Link&);

      static elle::Status       Bind(gear::Link&,
                                     const path::Way&);
      static elle::Status       Resolve(gear::Link&,
                                        path::Way&);

      static elle::Status       Discard(gear::Link&);
      static elle::Status       Store(gear::Link&);
      static elle::Status       Destroy(gear::Link&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Link.hh>

#endif
