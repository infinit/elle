//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/Janitor.hxx
//
// created       julien quintard   [wed sep  7 12:43:43 2011]
// updated       julien quintard   [wed sep  7 12:44:30 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/Etoile.hh>

namespace pig
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
    etoile::wall::Object::Discard(identifier);
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
