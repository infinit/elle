//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/core/Core.cc
//
// created       julien quintard   [thu jan 28 22:01:03 2010]
// updated       julien quintard   [mon feb  1 00:40:44 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Core.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the core by registering the component
    /// types to the factory.
    ///
    Status		Core::Initialize()
    {
      // register the component types.
      Factory::Register< Object >(Object::Identifier);
      Factory::Register< Contents<Data> >(Contents<Data>::Identifier);
      Factory::Register< Contents<Catalog> >(Contents<Catalog>::Identifier);
      Factory::Register< Contents<Reference> >(Contents<Reference>::Identifier);

      leave();
    }

    ///
    /// this method cleans the core.
    ///
    Status		Core::Clean()
    {
      // XXX

      leave();
    }

  }
}

