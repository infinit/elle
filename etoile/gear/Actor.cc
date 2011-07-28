//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Actor.cc
//
// created       julien quintard   [thu jul 28 13:05:00 2011]
// updated       julien quintard   [thu jul 28 13:06:35 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Actor.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Actor::Create(Scope*			scope)
    {
      enter();

      // set the scope.
      this->scope = scope;

      // generate an identifier.
      if (this->identifier.Generate() == elle::StatusError)
	escape("unable to generate the identifier");

      // XXX export i.e scope->Add

      leave();
    }

  }
}
