//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Kernel.cc
//
// created       julien quintard   [thu jan 28 22:01:03 2010]
// updated       julien quintard   [mon may  3 16:35:00 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Kernel.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the kernel by registering the component
    /// types to the factory.
    ///
    elle::Status	Kernel::Initialize()
    {
      enter();

      // register the component types.
      elle::Factory::Register< Object >(Object::Name);
      elle::Factory::Register< Contents<Data> >(Contents<Data>::Name);
      elle::Factory::Register< Contents<Catalog> >(Contents<Catalog>::Name);
      elle::Factory::Register< Contents<Reference> >(Contents<Reference>::Name);

      leave();
    }

    ///
    /// this method cleans the kernel.
    ///
    elle::Status	Kernel::Clean()
    {
      enter();

      // XXX

      leave();
    }

  }
}
