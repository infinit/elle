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
// updated       julien quintard   [wed mar  3 17:18:38 2010]
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
      enter();

      // register the component types.
      Factory::Register< Object >(Object::Name);
      Factory::Register< Contents<Data> >(Contents<Data>::Name);
      Factory::Register< Contents<Catalog> >(Contents<Catalog>::Name);
      Factory::Register< Contents<Reference> >(Contents<Reference>::Name);

      leave();
    }

    ///
    /// this method cleans the core.
    ///
    Status		Core::Clean()
    {
      enter();

      // XXX

      leave();
    }

  }
}
