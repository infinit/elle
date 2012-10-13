#ifndef HORIZON_JANITOR_HXX
# define HORIZON_JANITOR_HXX

#include <elle/types.hh>
#include <elle/standalone/Report.hh>

#include <etoile/wall/Object.hh>

#include <elle/idiom/Open.hh>

namespace horizon
{

//
// ---------- template methods ------------------------------------------------
//

  ///
  /// this method discards the potential modifications having been applied
  /// onto the given identified context.
  ///
  template <typename T>
  elle::Void          Janitor::Clear(T&               identifier)
  {
    // discard the identifier.
    etoile::wall::Object::Discard(identifier);
  }

  ///
  /// this method clears a set of identifiers.
  ///
  template <typename T,
            typename... TT>
  elle::Void          Janitor::Clear(T&               identifier,
                                     TT&...           identifiers)
  {
    Janitor::Clear(identifier);
    Janitor::Clear(identifiers...);
  }

}

#endif
