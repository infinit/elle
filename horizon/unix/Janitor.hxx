//
// ---------- header ----------------------------------------------------------
//
// project       facade
//
// license       infinit
//
// author        julien quintard   [wed sep  7 12:43:43 2011]
//

#ifndef FACADE_UNIX_JANITOR_HXX
#define FACADE_UNIX_JANITOR_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/Etoile.hh>

namespace facade
{
#undef unix
  namespace unix
  {

//
// ---------- template methods ------------------------------------------------
//

    ///
    /// this method discards the potential modifications having been applied
    /// onto the given identified context.
    ///
    template <typename T>
    elle::Void		Janitor::Clear(T&		identifier)
    {
      // discard the identifier.
      etoile::wall::Object::Discard(identifier);

      // should an error occur, purge the error messages.
      purge();
    }

    ///
    /// this method clears a set of identifiers.
    ///
    template <typename T,
	      typename... TT>
    elle::Void		Janitor::Clear(T&		identifier,
				       TT&...		identifiers)
    {
      Janitor::Clear(identifier);
      Janitor::Clear(identifiers...);
    }

  }
}

#endif
