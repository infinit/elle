//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/Nucleus.cc
//
// created       julien quintard   [thu jan 28 22:01:03 2010]
// updated       julien quintard   [fri may 13 10:54:47 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/Nucleus.hh>

namespace nucleus
{

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the nucleus by registering the component
  /// types to the factory.
  ///
  elle::Status		Nucleus::Initialize()
  {
    enter();

    // register the component types.
    elle::Factory::Register< neutron::Object >
      (neutron::ComponentObject);
    elle::Factory::Register< neutron::Contents<neutron::Data> >
      (neutron::ComponentData);
    elle::Factory::Register< neutron::Contents<neutron::Catalog> >
      (neutron::ComponentCatalog);
    elle::Factory::Register< neutron::Contents<neutron::Reference> >
      (neutron::ComponentReference);
    elle::Factory::Register< neutron::Access >
      (neutron::ComponentAccess);

    leave();
  }

  ///
  /// this method cleans the nucleus
  ///
  elle::Status		Nucleus::Clean()
  {
    enter();

    // XXX

    leave();
  }

}
